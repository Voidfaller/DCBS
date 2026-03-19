#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "gyro_driver.h"
#include "touch_driver.h"
#include "state_estimator.h"

static const char *TAG = "IMU_TEST";

//Queue handle
static QueueHandle_t imuQueue = NULL;

// IMU task: leest sensor en stopt data in queue
void imu_task(void *pvParameters)
{
    state_estimator_init();

    gyro_data_t gyro;
    accel_data_t accel;
    imu_data_t imu; // gebruik struct uit gyro_driver.h

    float dt = 0.02f; // 50 Hz

    while (1)
    {
        if (gyro_driver_read_gyro(&gyro) == ESP_OK &&
            gyro_driver_read_accel(&accel) == ESP_OK)
        {
            imu.gyro = gyro;
            imu.accel = accel;

            // Stop in queue
            if (xQueueSend(imuQueue, &imu, pdMS_TO_TICKS(10)) != pdPASS)
            {
                ESP_LOGW(TAG, "Queue full, IMU data lost");
            }
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read IMU");
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // 50 Hz
    }
}

// PID / state estimation task: haalt data uit queue
void pid_task(void *pvParameters)
{
    imu_data_t imu;   // gebruik struct uit gyro_driver.h
    orientation_t orient;
    float dt = 0.02f;

    while (1)
    {
        if (xQueueReceive(imuQueue, &imu, portMAX_DELAY) == pdPASS)
        {
            // Update orientation
            state_estimator_update(&imu.gyro, &imu.accel, dt);
            orient = state_estimator_get_orientation();

            // Log de resultaten
            ESP_LOGI(TAG, "Pitch: %.2f°, Roll: %.2f°", orient.pitch, orient.roll);
            //volgende stap:
            //TODO: hier kan PID output voor servo komen
        }
    }
}


void app_main(void)
{
  
    if (touch_driver_init() != ESP_OK) {
        ESP_LOGE(TAG, "touch_driver_init failed");
        return;
    }

    xTaskCreate(touch_driver_task, "touch_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Touch test started");
}