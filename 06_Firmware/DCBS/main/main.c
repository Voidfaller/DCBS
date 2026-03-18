#include <stdio.h>
#include "servo_driver.h"
#include "gyro_driver.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Logging started"); //For filter use: "idf.py monitor --print_filter="[driver]:[loglevel]"" e.g --print_filter="gyro:I"
    servo_driver_init();
    gyro_driver_init();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    
    }
}
