#pragma once

#include <stdint.h>
#include "esp_err.h"

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

esp_err_t gyro_driver_init(void); // esp32 start I2C 

esp_err_t gyro_driver_read_gyro(gyro_data_t *data);

esp_err_t gyro_driver_read_accel(accel_data_t *data);
