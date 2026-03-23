#pragma once

#include "gyro_driver.h"

typedef struct
{
    float pitch;
    float roll;
} orientation_t;

void state_estimator_init(void);

void state_estimator_update(
    imu_data_t *data,
    float dt);

orientation_t state_estimator_get_orientation(void);

void state_estimator_task(void *arg);
esp_err_t state_estimator_start(void);
