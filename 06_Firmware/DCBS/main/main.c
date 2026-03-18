#include <stdio.h>
#include "servo_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "gyro_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "TEST";

void app_main(void)
{
    gyro_data_t gyro;
    accel_data_t accel;

    if (gyro_driver_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init MPU6050");
        return;
    }

    ESP_LOGI(TAG, "MPU6050 initialized");

    while (1)
    {
        if (gyro_driver_read_gyro(&gyro) == ESP_OK &&
            gyro_driver_read_accel(&accel) == ESP_OK)
        {
            ESP_LOGI(TAG,
                "GYRO: X=%.2f Y=%.2f Z=%.2f | ACCEL: X=%.2f Y=%.2f Z=%.2f",
                gyro.gyro_x, gyro.gyro_y, gyro.gyro_z,
                accel.accel_x, accel.accel_y, accel.accel_z);
        }
        else
        {
            ESP_LOGE(TAG, "Read error");
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}