#include <stdbool.h>
#include <stdint.h>

#include "sleep_module.h"
#include "display_driver.h"
#include "driver/touch_pad.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_SLEEP_TIMER_TASKS 2
#define DEFAULT_TIMEOUT_MS 30000U

typedef struct
{
    const char *name;
    uint32_t time_until_sleep_ms;
    TickType_t last_awake_call;
    bool active;
} sleep_timer_entry_t;

static sleep_timer_entry_t s_tasks[MAX_SLEEP_TIMER_TASKS];
static int s_task_count = 0;
static TaskHandle_t s_sleep_task_handle = NULL;
static uint32_t s_default_timeout_ms = DEFAULT_TIMEOUT_MS;
static const char *TAG = "SLEEP_MODULE";

#define WAKE_TOUCH_PAD TOUCH_PAD_NUM0

static void sleep_timer_arm_touch_wakeup(void)
{
    const uint16_t touch_bit = (uint16_t)(1U << (uint32_t)WAKE_TOUCH_PAD);

    (void)touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
    (void)touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_SET1);
    (void)touch_pad_set_group_mask(touch_bit, 0, touch_bit);
    (void)touch_pad_clear_status();
    (void)touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
}

static void sleep_timer_mark_awake_all(void)
{
    TickType_t now = xTaskGetTickCount();

    for (int i = 0; i < s_task_count; i++)
    {
        if (s_tasks[i].active)
        {
            s_tasks[i].last_awake_call = now;
        }
    }
}

static bool sleep_timer_should_sleep(TickType_t now)
{
    for (int i = 0; i < s_task_count; i++)
    {
        if (!s_tasks[i].active)
        {
            continue;
        }

        uint32_t elapsed_ms = (uint32_t)((now - s_tasks[i].last_awake_call) * portTICK_PERIOD_MS);
        if (elapsed_ms >= s_tasks[i].time_until_sleep_ms)
        {
            return true;
        }
    }

    return false;
}

static void sleep_timer_configure_wakeup(void)
{
    esp_err_t err = esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to keep RTC peripheral domain ON for touch wake: %s", esp_err_to_name(err));
    }

    (void)esp_sleep_enable_touchpad_wakeup();
}

sleep_timer_handle_t sleep_timer_register(const char *name, uint32_t stay_awake_sec)
{
    if (s_task_count >= MAX_SLEEP_TIMER_TASKS)
    {
        return -1;
    }

    s_tasks[s_task_count].name = name;
    s_tasks[s_task_count].time_until_sleep_ms = stay_awake_sec * 1000U;
    s_tasks[s_task_count].last_awake_call = xTaskGetTickCount();
    s_tasks[s_task_count].active = true;

    return s_task_count++;
}

void sleep_timer_reset(void)
{
    sleep_timer_mark_awake_all();
}

void sleep_timer_task(void *arg)
{
    (void)arg;

    while (1)
    {
        TickType_t now = xTaskGetTickCount();
        if (sleep_timer_should_sleep(now))
        {
            ESP_LOGW(TAG, "Inactivity timeout reached, entering deep sleep now");
            sleep_timer_arm_touch_wakeup();
            (void)display_driver_prepare_sleep();
            vTaskDelay(pdMS_TO_TICKS(20));
            esp_deep_sleep_start();
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void sleep_timer_init(uint32_t time_until_sleep_sec)
{
    uint32_t timeout_ms = time_until_sleep_sec == 0 ? DEFAULT_TIMEOUT_MS : time_until_sleep_sec * 1000U;

    sleep_timer_configure_wakeup();

    s_default_timeout_ms = timeout_ms;
    if (s_task_count == 0)
    {
        s_tasks[0].name = "sleep_timer";
        s_tasks[0].time_until_sleep_ms = s_default_timeout_ms;
        s_tasks[0].last_awake_call = xTaskGetTickCount();
        s_tasks[0].active = true;
        s_task_count = 1;
    }
    else
    {
        s_tasks[0].time_until_sleep_ms = s_default_timeout_ms;
        s_tasks[0].last_awake_call = xTaskGetTickCount();
        s_tasks[0].active = true;
    }

    if (s_sleep_task_handle == NULL)
    {
        xTaskCreate(sleep_timer_task, "sleep_timer_task", 2048, NULL, 10, &s_sleep_task_handle);
    }
}