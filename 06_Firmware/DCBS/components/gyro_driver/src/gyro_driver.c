#include "gyro_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2C_wrapper.h"
#include "esp_timer.h"
#include "watchdog_service.h"
#define MPU6050_ADDR 0x68 // I2C address of the MPU6050
// MPU6050 register addressed
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H 0x43
// sampling and timing configuration
#define GYRO_SAMPLE_PERIOD_MS 10
#define GYRO_I2C_TIMEOUT_MS 80
#define GYRO_TASK_STACK_SIZE 4096U
#define GYRO_ERROR_LOG_INTERVAL_MS 1000U
#define GYRO_PERF_LOG_INTERVAL_US (2000000ULL)
// queue to send IMU data to other modules
QueueHandle_t gyro_queue_handle = NULL;
static QueueHandle_t s_gyro_monitor_queue_handle = NULL;

static i2c_master_dev_handle_t s_mpu6050_dev = NULL; // I2C device handle for MPU6050
static TaskHandle_t s_gyro_task_handle = NULL; // task handle 

static watchdog_handle_t s_watchdog_handle = WATCHDOG_INVALID_HANDLE; // watchdog handle
// return monitor queue
QueueHandle_t gyro_driver_get_monitor_queue_handle(void) 
{
    return s_gyro_monitor_queue_handle;
}
// initialize MPU6050 sensor
esp_err_t gyro_driver_init(void)
{
    esp_err_t err;
    // ensure I2C bus is initialized before trying to add the device
    if (!i2c_wrapper_is_initialized())
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_mpu6050_dev != NULL) // prevent re-initialization
    {
        return ESP_OK;
    }
    // add MPU6050 device to I2C bus
    err = i2c_wrapper_add_device(MPU6050_ADDR, 0, &s_mpu6050_dev);
    if (err != ESP_OK)
    {
        return err;
    }

    // wake up the MPU6050 : clear sleep bit in PWR_MGMT_1 register
    return i2c_wrapper_write_reg_u8(s_mpu6050_dev, MPU6050_PWR_MGMT_1, 0x00, GYRO_I2C_TIMEOUT_MS);
}
// Read gyro data 
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
    // read 6 bites of gyro x,y,z gyro
    err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_GYRO_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    // retry once if timeout occurs
    if (err == ESP_ERR_TIMEOUT)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_GYRO_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    }

    if (err != ESP_OK)
        return err;
    // combine high and low bytes into signed 16 bit values
    int16_t gx = (buffer[0] << 8) | buffer[1]; // accel x
    int16_t gy = (buffer[2] << 8) | buffer[3]; // accel y
    int16_t gz = (buffer[4] << 8) | buffer[5]; // accel z
    // convert raw values to degrees per second
    data->gyro_x = gx / 131.0f;
    data->gyro_y = gy / 131.0f;
    data->gyro_z = gz / 131.0f;

    return ESP_OK;
}
// read acceleromter data
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
// read 6 bytes of accel x,y,z data
    err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_ACCEL_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    // retry once if timeout occurs
    if (err == ESP_ERR_TIMEOUT)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        err = i2c_wrapper_read_reg(s_mpu6050_dev, MPU6050_ACCEL_XOUT_H, buffer, sizeof(buffer), GYRO_I2C_TIMEOUT_MS);
    }

    if (err != ESP_OK)
        return err;
    // combine high and low bytes into signed 16 bit values
    int16_t ax = (buffer[0] << 8) | buffer[1]; // accel x
    int16_t ay = (buffer[2] << 8) | buffer[3]; // accel y
    int16_t az = (buffer[4] << 8) | buffer[5]; // accel z
    // convert raw values to g
    data->accel_x = ax / 16384.0f;
    data->accel_y = ay / 16384.0f;
    data->accel_z = az / 16384.0f;

    return ESP_OK;
}
// start gyro driver task
esp_err_t gyro_driver_start(void)
{
    BaseType_t task_ok;
    // prevent multiple instances of the task from being created
    if (s_gyro_task_handle != NULL)
    {
        return ESP_OK;
    }
    // create main data queue if it doesn't exist
    if (gyro_queue_handle == NULL)
    {
        gyro_queue_handle = xQueueCreate(1, sizeof(imu_data_t));
        if (gyro_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }
    // create optional monitor/debug queue 
    if (s_gyro_monitor_queue_handle == NULL)
    {
        s_gyro_monitor_queue_handle = xQueueCreate(1, sizeof(imu_data_t));
        if (s_gyro_monitor_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }
    // create Freetos task
    task_ok = xTaskCreate(gyro_driver_task, "gyro_task", GYRO_TASK_STACK_SIZE, NULL, 10, &s_gyro_task_handle);
    if (task_ok != pdPASS)
    {
        s_gyro_task_handle = NULL;
        return ESP_FAIL;
    }
    // register wachtdog
    s_watchdog_handle = watchdog_register("Gyro Driver", 5);
    return ESP_OK;
}
// main gyro driver task loop: read sensor data and send to queue
void gyro_driver_task(void *arg)
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
    // ensure queue exists before starting loop
    if (gyro_queue_handle == NULL)
    {
        printf("Gyro queue is not initialized\n");
        s_gyro_task_handle = NULL;
        vTaskDelete(NULL);
        return;
    }
    // performance tracking variables
    uint64_t perf_last_log_us = esp_timer_get_time();
    uint64_t perf_max_loop_us = 0;
    uint64_t perf_max_read_us = 0;
    uint32_t perf_timeouts = 0;
    TickType_t last_error_log_tick = 0;

    while (1)
    {
        uint64_t loop_start_us = esp_timer_get_time();
        uint64_t read_start_us;
        uint64_t read_end_us;

        imu_data_t data;
        read_start_us = esp_timer_get_time(); // read gyro data
        err = gyro_driver_read_gyro(&data.gyro);
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_TIMEOUT)
            {
                perf_timeouts++;
            }
            // rate limited error logging
            TickType_t now_tick = xTaskGetTickCount();
            if ((now_tick - last_error_log_tick) >= pdMS_TO_TICKS(GYRO_ERROR_LOG_INTERVAL_MS))
            {
                printf("Failed to read gyro: %s\n", esp_err_to_name(err));
                last_error_log_tick = now_tick;
            }
            // attempt recovery if device handle is lost or invalid
            if (err == ESP_ERR_INVALID_STATE)
            {
                // Try to recover if the device handle was lost or became invalid.
                s_mpu6050_dev = NULL;
                (void)gyro_driver_init();
            }

            watchdog_feed(s_watchdog_handle); // feed watchdog before delay to prevent reset during error conditions
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        // read accel data 
        err = gyro_driver_read_accel(&data.accel);
        read_end_us = esp_timer_get_time();
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_TIMEOUT) // track timeouts for performance monitoring
            {
                perf_timeouts++;
            }

            TickType_t now_tick = xTaskGetTickCount();
            if ((now_tick - last_error_log_tick) >= pdMS_TO_TICKS(GYRO_ERROR_LOG_INTERVAL_MS))
            {
                printf("Failed to read accel: %s\n", esp_err_to_name(err));
                last_error_log_tick = now_tick;
            }

            if (err == ESP_ERR_INVALID_STATE) // try to recover if device handle is invalid
            {
                s_mpu6050_dev = NULL; // clear handle to force re-intialization 
                (void)gyro_driver_init();
            }

            watchdog_feed(s_watchdog_handle);
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        // track read performance
        uint64_t read_us = read_end_us - read_start_us;
        if (read_us > perf_max_read_us)
        {
            perf_max_read_us = read_us;
        }
        // send data to main queue
        if (gyro_queue_handle != NULL)
        {
            xQueueOverwrite(gyro_queue_handle, &data);
            // trace_div++;
            // if (trace_div >= 20U)
            // {
            //     printf("[gyro] gyro=(%.2f, %.2f, %.2f) accel=(%.2f, %.2f, %.2f)\n",
            //            data.gyro.gyro_x, data.gyro.gyro_y, data.gyro.gyro_z,
            //            data.accel.accel_x, data.accel.accel_y, data.accel.accel_z);
            //     trace_div = 0U;
            // }
        }
// send data to monitor/debug queue 
        if (s_gyro_monitor_queue_handle != NULL)
        {
            xQueueOverwrite(s_gyro_monitor_queue_handle, &data);
        }

     
// feed watchdog and delay until next sample period 
        watchdog_feed(s_watchdog_handle);
        vTaskDelay(pdMS_TO_TICKS(GYRO_SAMPLE_PERIOD_MS));
    }
}