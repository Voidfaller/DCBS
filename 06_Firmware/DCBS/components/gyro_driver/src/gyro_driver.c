#include "gyro_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2C_wrapper.h"
#include "esp_timer.h"
#include "watchdog_service.h"

#define MPU6050_ADDR 0x68

#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H 0x43
#define GYRO_SAMPLE_PERIOD_MS 10
#define GYRO_I2C_TIMEOUT_MS 80
#define GYRO_TASK_STACK_SIZE 4096U
#define GYRO_ERROR_LOG_INTERVAL_MS 1000U
#define GYRO_PERF_LOG_INTERVAL_US (2000000ULL)

static QueueHandle_t s_gyro_queue_handle = NULL;
static QueueHandle_t s_gyro_monitor_queue_handle = NULL;

static i2c_master_dev_handle_t s_mpu6050_dev = NULL;
static TaskHandle_t s_gyro_task_handle = NULL;

static watchdog_handle_t s_watchdog_handle = WATCHDOG_INVALID_HANDLE;

static void gyro_driver_task(void *arg);

QueueHandle_t gyro_driver_get_queue_handle(void)
{
    return s_gyro_queue_handle;
}

QueueHandle_t gyro_driver_get_monitor_queue_handle(void)
{
    return s_gyro_monitor_queue_handle;
}

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

    return i2c_wrapper_write_reg_u8(s_mpu6050_dev, MPU6050_PWR_MGMT_1, 0x00, GYRO_I2C_TIMEOUT_MS);
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

    err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_GYRO_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    if (err == ESP_ERR_TIMEOUT)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_GYRO_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    }

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

    err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_ACCEL_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    if (err == ESP_ERR_TIMEOUT)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_ACCEL_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    }

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

    if (s_gyro_queue_handle == NULL)
    {
        s_gyro_queue_handle = xQueueCreate(1, sizeof(imu_data_t));
        if (s_gyro_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    if (s_gyro_monitor_queue_handle == NULL)
    {
        s_gyro_monitor_queue_handle = xQueueCreate(1, sizeof(imu_data_t));
        if (s_gyro_monitor_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    task_ok = xTaskCreate(gyro_driver_task, "gyro_task", GYRO_TASK_STACK_SIZE, NULL, 10, &s_gyro_task_handle);
    if (task_ok != pdPASS)
    {
        s_gyro_task_handle = NULL;
        return ESP_FAIL;
    }

    s_watchdog_handle = watchdog_register("Gyro Driver", 5);
    return ESP_OK;
}

static void gyro_driver_task(void *arg)
{
    // initialize the driver
    esp_err_t err;
    (void)arg;

    err = gyro_driver_init();
    if (err != ESP_OK)
    {
        printf("Gyro driver init failed: %s\n", esp_err_to_name(err));
        vTaskDelete(NULL);
        return;
    }

    if (s_gyro_queue_handle == NULL)
    {
        printf("Gyro queue is not initialized\n");
        s_gyro_task_handle = NULL;
        vTaskDelete(NULL);
        return;
    }

    uint64_t perf_last_log_us = esp_timer_get_time();
    uint64_t perf_max_loop_us = 0;
    uint64_t perf_max_read_us = 0;
    uint32_t perf_samples = 0;
    uint32_t perf_timeouts = 0;
    TickType_t last_error_log_tick = 0;

    while (1)
    {
        uint64_t loop_start_us = esp_timer_get_time();
        uint64_t read_start_us;
        uint64_t read_end_us;

        imu_data_t data;
        read_start_us = esp_timer_get_time();
        err = gyro_driver_read_gyro(&data.gyro);
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_TIMEOUT)
            {
                perf_timeouts++;
            }

            TickType_t now_tick = xTaskGetTickCount();
            if ((now_tick - last_error_log_tick) >= pdMS_TO_TICKS(GYRO_ERROR_LOG_INTERVAL_MS))
            {
                printf("Failed to read gyro: %s\n", esp_err_to_name(err));
                last_error_log_tick = now_tick;
            }

            if (err == ESP_ERR_INVALID_STATE)
            {
                // Try to recover if the device handle was lost or became invalid.
                s_mpu6050_dev = NULL;
                (void)gyro_driver_init();
            }

            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        err = gyro_driver_read_accel(&data.accel);
        read_end_us = esp_timer_get_time();
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_TIMEOUT)
            {
                perf_timeouts++;
            }

            TickType_t now_tick = xTaskGetTickCount();
            if ((now_tick - last_error_log_tick) >= pdMS_TO_TICKS(GYRO_ERROR_LOG_INTERVAL_MS))
            {
                printf("Failed to read accel: %s\n", esp_err_to_name(err));
                last_error_log_tick = now_tick;
            }

            if (err == ESP_ERR_INVALID_STATE)
            {
                s_mpu6050_dev = NULL;
                (void)gyro_driver_init();
            }

            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        uint64_t read_us = read_end_us - read_start_us;
        if (read_us > perf_max_read_us)
        {
            perf_max_read_us = read_us;
        }

        if (s_gyro_queue_handle != NULL)
        {
            xQueueOverwrite(s_gyro_queue_handle, &data);
            // printf("Gyro data sent to queue: gyro=(%.2f, %.2f, %.2f) accel=(%.2f, %.2f, %.2f)\n",
            //        data.gyro.gyro_x, data.gyro.gyro_y, data.gyro.gyro_z,
            //        data.accel.accel_x, data.accel.accel_y, data.accel.accel_z);
        }

        if (s_gyro_monitor_queue_handle != NULL)
        {
            xQueueOverwrite(s_gyro_monitor_queue_handle, &data);
        }

        uint64_t loop_us = esp_timer_get_time() - loop_start_us;
        if (loop_us > perf_max_loop_us)
        {
            perf_max_loop_us = loop_us;
        }

        perf_samples++;
        uint64_t now_us = esp_timer_get_time();
        if ((now_us - perf_last_log_us) >= GYRO_PERF_LOG_INTERVAL_US)
        {
            printf("GYRO PERF samples=%lu max_read_us=%llu max_loop_us=%llu timeouts=%lu\n",
                   (unsigned long)perf_samples,
                   (unsigned long long)perf_max_read_us,
                   (unsigned long long)perf_max_loop_us,
                   (unsigned long)perf_timeouts);
            perf_last_log_us = now_us;
            perf_max_read_us = 0;
            perf_max_loop_us = 0;
            perf_samples = 0;
            perf_timeouts = 0;
        }
        watchdog_feed(s_watchdog_handle);
        vTaskDelay(pdMS_TO_TICKS(GYRO_SAMPLE_PERIOD_MS));
    }
}