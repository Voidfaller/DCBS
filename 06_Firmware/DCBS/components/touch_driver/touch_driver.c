#include "touch_driver.h"
#include <stdint.h>
#include "driver/touch_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "TOUCH_DRIVER"; //logging tag


esp_err_t touch_driver_init(void)
{
    esp_err_t err = touch_pad_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    //init pin
    err = touch_pad_config(TOUCH_PIN, 0);
    if (err != ESP_OK)
    {
        return err;
    }

    // software trigger mode
    err = touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);
    if (err != ESP_OK)
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
    esp_err_t err = touch_pad_sw_start();
    if (err != ESP_OK)
    {
        return err;
    }

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

//FreeRTOS task to continuously read touch data and log the event
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

