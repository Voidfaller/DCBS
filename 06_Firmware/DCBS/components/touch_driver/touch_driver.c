#include "touch_driver.h"
#include <stdint.h>
#include "driver/touch_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "config_manager.h"
#include "state_estimator.h"

static const char *TAG = "TOUCH_DRIVER"; //logging tag
static TaskHandle_t s_touch_task_handle = NULL;


esp_err_t touch_driver_init(void)
{
    esp_err_t err = touch_pad_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    //init pin
    err = touch_pad_config(TOUCH_PIN, TOUCH_THRESHOLD);
    if (err != ESP_OK)
    {
        return err;
    }

    // Timer FSM mode is required for touch wakeup from deep sleep.
    err = touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    if (err != ESP_OK)
    {
        return err;
    }

    err = touch_pad_fsm_start();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }
    return ESP_OK;
}

esp_err_t touch_driver_read(touch_data_t *data)
{

    if (data == NULL)
    {
 return ESP_ERR_INVALID_ARG;
    }

    uint16_t raw_value = 0;
    esp_err_t err;
    err = touch_pad_read(TOUCH_PIN, &raw_value);
    if (err != ESP_OK)
    {
        return err;
    }
// determine touch status based on threshold
    data->touch_value = raw_value;
    data->touched = raw_value < TOUCH_THRESHOLD;

    return ESP_OK;
}

// Starts the touch driver task that continuously reads touch data and updates setpoints on touch events, supposedly
esp_err_t touch_driver_start(void)
{
    BaseType_t task_ok;

    if (s_touch_task_handle != NULL)
    {
        return ESP_OK;
    }

    if (touch_driver_init() != ESP_OK)
    {
        return ESP_FAIL;
    }

    task_ok = xTaskCreate(touch_driver_task, "touch_driver_task", 2048, NULL, 5, &s_touch_task_handle);
    if (task_ok != pdPASS)
    {
        s_touch_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

//FreeRTOS task to continuously read touch data and log the event.
// NEW : On touch events it also captures the current orientation from the state estimator and updates the setpoints in the config manager 
void touch_driver_task(void *pvParameters)
{
    touch_data_t data;
    bool was_touched = false;

    ESP_LOGI(TAG, "Touch driver is listening now!");

    while (1)
    {
        if (touch_driver_read(&data) == ESP_OK)
        {
        // Log only on touch events, not on every read
            if (data.touched && !was_touched)
            {
                ESP_LOGI(TAG, "Touch detected (value: %lu)", (unsigned long)data.touch_value);
        // Capture current orientation and set it as the new setpoint in the config manager
                orientation_t orientation = state_estimator_get_orientation();
                if (config_manager_set_current_setpoints(orientation.pitch, orientation.roll, true) == ESP_OK)
                {
                    ESP_LOGI(TAG, "Captured! >Current setpoints: pitch=%.2f roll=%.2f", orientation.pitch, orientation.roll);
                    sleep_timer_reset();
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to store setpoints");
                }
            }
        // Update the previous touch state
            was_touched = data.touched;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read touch data");
        }

        vTaskDelay(pdMS_TO_TICKS(TOUCH_POLL_MS));
    }
}

