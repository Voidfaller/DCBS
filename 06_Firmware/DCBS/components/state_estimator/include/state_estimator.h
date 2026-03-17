#pragma once

#include "gyro_driver.h"

typedef struct
{
    float pitch;
    float roll;
} orientation_t;

void state_estimator_init(void);

void state_estimator_update(
    const gyro_data_t *gyro,
    const accel_data_t *accel,
    float dt);

orientation_t state_estimator_get_orientation(void);