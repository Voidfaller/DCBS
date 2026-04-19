#include "touch_driver.h"

#include <stdint.h>
#include "driver/touch_pad.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

static const char *TAG = "TOUCH_DRIVER";
static TaskHandle_t s_touch_task_handle = NULL;
static volatile uint16_t s_last_touch_value = 0;
static volatile uint64_t s_last_activity_us = 0;
static uint64_t s_last_touch_log_us = 0;

#define TOUCH_TASK_STACK_SIZE 4096
#define TOUCH_ACTIVITY_LOG_INTERVAL_US 250000ULL

static void touch_enter_deep_sleep(void)
{
    // Touch wakeup must be armed before entering deep sleep.
    esp_err_t err = esp_sleep_enable_touchpad_wakeup();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to enable touch wakeup: %s", esp_err_to_name(err));
        return;
    }

    // No recent touch activity means the device can safely suspend until a pad wakes it.
    ESP_LOGI(TAG, "No touch activity for %u ms, entering deep sleep", (unsigned)TOUCH_SLEEP_TIMEOUT_MS);
    esp_deep_sleep_start();
}

static void IRAM_ATTR touch_isr_handler(void *arg)
{
    (void)arg;
    // Keep the ISR short: clear the hardware status and notify the task that a touch occurred.
    BaseType_t higher_priority_task_woken = pdFALSE;
    uint32_t pad_status = touch_pad_get_status();
    touch_pad_clear_status();

    if ((pad_status & (1UL << TOUCH_PIN)) != 0U)
    {
        s_last_activity_us = (uint64_t)esp_timer_get_time();

        if (s_touch_task_handle != NULL)
        {
            vTaskNotifyGiveFromISR(s_touch_task_handle, &higher_priority_task_woken);
        }
    }

    if (higher_priority_task_woken == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

esp_err_t touch_driver_init(void)
{
    // Initialize the touch peripheral and configure a single pad as the activity source.
    esp_err_t err = touch_pad_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    err = touch_pad_config(TOUCH_PIN, TOUCH_THRESHOLD);
    if (err != ESP_OK)
    {
        return err;
    }

    err = touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
    if (err != ESP_OK)
    {
        return err;
    }

    err = touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_SET1);
    if (err != ESP_OK)
    {
        return err;
    }

    err = touch_pad_set_group_mask((1UL << TOUCH_PIN), 0, (1UL << TOUCH_PIN));
    if (err != ESP_OK)
    {
        return err;
    }

    err = touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    if (err != ESP_OK)
    {
        return err;
    }

    err = touch_pad_isr_register(touch_isr_handler, NULL);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    err = touch_pad_intr_enable();
    if (err != ESP_OK)
    {
        return err;
    }

    touch_pad_clear_status();

    err = touch_pad_sw_start();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    // Seed the inactivity timer so startup does not immediately trip deep sleep.
    s_last_activity_us = (uint64_t)esp_timer_get_time();
    return ESP_OK;
}

esp_err_t touch_driver_read(touch_data_t *data)
{
    uint16_t raw_value = 0;

    esp_err_t err;

    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    err = touch_pad_read(TOUCH_PIN, &raw_value);
    if (err != ESP_OK)
    {
        return err;
    }

    data->touch_value = raw_value;
    data->touched = raw_value < TOUCH_THRESHOLD;
    return ESP_OK;
}

esp_err_t touch_driver_start(void)
{
    BaseType_t task_ok;

    if (s_touch_task_handle != NULL)
    {
        return ESP_OK;
    }

    // The driver is initialized once and then driven by the background task.
    if (touch_driver_init() != ESP_OK)
    {
        return ESP_FAIL;
    }

    task_ok = xTaskCreate(touch_driver_task, "touch_driver_task", TOUCH_TASK_STACK_SIZE, NULL, 5, &s_touch_task_handle);
    if (task_ok != pdPASS)
    {
        s_touch_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

void touch_driver_task(void *pvParameters)
{
    (void)pvParameters;
    touch_data_t data;

    // The task waits for touch notifications, samples the pad, and enforces the idle timeout.
    ESP_LOGI(TAG, "Touch interrupt + inactivity timeout active (%u ms)", (unsigned)TOUCH_SLEEP_TIMEOUT_MS);

    while (1)
    {
        // Sleep until the ISR notifies us or a short poll interval expires.
        (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

        if (touch_driver_read(&data) == ESP_OK && data.touched)
        {
            s_last_touch_value = (uint16_t)data.touch_value;
            s_last_activity_us = (uint64_t)esp_timer_get_time();
            if ((s_last_activity_us - s_last_touch_log_us) >= TOUCH_ACTIVITY_LOG_INTERVAL_US)
            {
                ESP_LOGI(TAG, "Touch activity (value: %u)", (unsigned)s_last_touch_value);
                s_last_touch_log_us = s_last_activity_us;
            }
        }

        if (((uint64_t)esp_timer_get_time() - s_last_activity_us) >= ((uint64_t)TOUCH_SLEEP_TIMEOUT_MS * 1000ULL))
        {
            touch_enter_deep_sleep();
        }
    }
}

