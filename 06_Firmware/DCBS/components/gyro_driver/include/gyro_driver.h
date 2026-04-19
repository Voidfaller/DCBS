#pragma once

//libraries
#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// typedefs
typedef struct
{
    float gyro_x;
    float gyro_y;
    float gyro_z;
} gyro_data_t;

typedef struct
{
    float accel_x;
    float accel_y;
    float accel_z;
} accel_data_t;
typedef struct
{
    gyro_data_t gyro;
    accel_data_t accel;
} imu_data_t;

//globals
extern QueueHandle_t gyro_queue_handle;

//initialization and state
esp_err_t gyro_driver_init(void);
esp_err_t gyro_driver_start(void);
void gyro_driver_task(void *arg);

//accessors
esp_err_t gyro_driver_read_gyro(gyro_data_t *data);
esp_err_t gyro_driver_read_accel(accel_data_t *data);
QueueHandle_t gyro_driver_get_monitor_queue_handle(void);