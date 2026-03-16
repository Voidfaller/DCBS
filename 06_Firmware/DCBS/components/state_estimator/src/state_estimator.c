#include "state_estimator.h"
#include "gyro_driver.h"
#include <math.h>

#define ALPHA 0.98f

static state_estimate_t current_state;

static gyro_data_t gyro;
static accel_data_t accel;

esp_err_t state_estimator_init(void)
{
    current_state.roll = 0.0f;
    current_state.pitch = 0.0f;

    return ESP_OK;
}

esp_err_t state_estimator_update(float dt)
{
    esp_err_t err;

    err = gyro_driver_read_gyro(&gyro);
    if (err != ESP_OK)
        return err;

    err = gyro_driver_read_accel(&accel);
    if (err != ESP_OK)
        return err;

    float accel_roll = atan2f(accel.accel_y, accel.accel_z) * 180.0f / M_PI;
    float accel_pitch = atan2f(-accel.accel_x,
                               sqrtf(accel.accel_y * accel.accel_y +
                                     accel.accel_z * accel.accel_z)) * 180.0f / M_PI;

    float gyro_roll = current_state.roll + gyro.gyro_x * dt;
    float gyro_pitch = current_state.pitch + gyro.gyro_y * dt;

    current_state.roll =
        ALPHA * gyro_roll +
        (1.0f - ALPHA) * accel_roll;

    current_state.pitch =
        ALPHA * gyro_pitch +
        (1.0f - ALPHA) * accel_pitch;

    return ESP_OK;
}

state_estimate_t state_estimator_get_state(void)
{
    return current_state;
}