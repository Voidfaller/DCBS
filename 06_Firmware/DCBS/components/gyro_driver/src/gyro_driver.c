#include "gyro_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2C_wrapper.h"

#define MPU6050_ADDR 0x68

#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H 0x43

QueueHandle_t gyro_queue_handle = NULL;

static i2c_master_dev_handle_t s_mpu6050_dev = NULL;
static TaskHandle_t s_gyro_task_handle = NULL;

esp_err_t gyro_driver_init(void)
{
    esp_err_t err;

    if (!i2c_wrapper_is_initialized())
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_mpu6050_dev != NULL)
    {
        return ESP_OK;
    }

    err = i2c_wrapper_add_device(MPU6050_ADDR, 0, &s_mpu6050_dev);
    if (err != ESP_OK)
    {
        return err;
    }

    return i2c_wrapper_write_reg_u8(s_mpu6050_dev, MPU6050_PWR_MGMT_1, 0x00, 100);
}

esp_err_t gyro_driver_read_gyro(gyro_data_t *data)
{
    uint8_t buffer[6];
    esp_err_t err;

    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_mpu6050_dev == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_GYRO_XOUT_H, buffer, sizeof(buffer), 100);

    if (err != ESP_OK)
        return err;

    int16_t gx = (buffer[0] << 8) | buffer[1];
    int16_t gy = (buffer[2] << 8) | buffer[3];
    int16_t gz = (buffer[4] << 8) | buffer[5];

    data->gyro_x = gx / 131.0f;
    data->gyro_y = gy / 131.0f;
    data->gyro_z = gz / 131.0f;

    return ESP_OK;
}

esp_err_t gyro_driver_read_accel(accel_data_t *data)
{
    uint8_t buffer[6];
    esp_err_t err;

    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_mpu6050_dev == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_ACCEL_XOUT_H, buffer, sizeof(buffer), 100);

    if (err != ESP_OK)
        return err;

    int16_t ax = (buffer[0] << 8) | buffer[1];
    int16_t ay = (buffer[2] << 8) | buffer[3];
    int16_t az = (buffer[4] << 8) | buffer[5];

    data->accel_x = ax / 16384.0f;
    data->accel_y = ay / 16384.0f;
    data->accel_z = az / 16384.0f;

    return ESP_OK;
}

esp_err_t gyro_driver_start(void)
{
    BaseType_t task_ok;

    if (s_gyro_task_handle != NULL)
    {
        return ESP_OK;
    }

    if (gyro_queue_handle == NULL)
    {
        gyro_queue_handle = xQueueCreate(10, sizeof(imu_data_t));
        if (gyro_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    task_ok = xTaskCreate(gyro_driver_task, "gyro_task", 2048, NULL, 10, &s_gyro_task_handle);
    if (task_ok != pdPASS)
    {
        s_gyro_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

void gyro_driver_task(void *arg)
{

    //initialize the driver
    esp_err_t err;
    (void)arg;

    err = gyro_driver_init();
    if (err != ESP_OK)
    {
        printf("Gyro driver init failed: %s\n", esp_err_to_name(err));
        vTaskDelete(NULL);
        return;
    }


    if (gyro_queue_handle == NULL)
    {
        printf("Gyro queue is not initialized\n");
        s_gyro_task_handle = NULL;
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
       imu_data_t data;
        err = gyro_driver_read_gyro(&data.gyro);
        if (err != ESP_OK)
        {
            printf("Failed to read gyro: %s\n", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        err = gyro_driver_read_accel(&data.accel);
        if (err != ESP_OK)
        {
            printf("Failed to read accel: %s\n", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (gyro_queue_handle != NULL)
        {
            xQueueSend(gyro_queue_handle, &data, pdMS_TO_TICKS(10));
        }
    }
}