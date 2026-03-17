#include "state_estimator.h"
#include <math.h>

#define RAD_TO_DEG 57.2958f
// Pid controller krijgt dit later pitch and roll
// bijvoorbeeld pitch = +5° → servo corrigeert
// roll = -3° → andere servo corrigeert
// complementary filter coefficient
#define COMPLEMENTARY_ALPHA 0.98f

static orientation_t orientation;

void state_estimator_init(void)
{
    orientation.pitch = 0.0f;
    orientation.roll = 0.0f;
}

void state_estimator_update(
    const gyro_data_t *gyro,
    const accel_data_t *accel,
    float dt)
{
    // Gyroscope integration
    float gyro_pitch = orientation.pitch + gyro->gyro_y * dt;
    float gyro_roll  = orientation.roll  + gyro->gyro_x * dt;

    // Accelerometer angle calculation
    float accel_pitch = atan2f(
        accel->accel_x,
        sqrtf(accel->accel_y * accel->accel_y +
              accel->accel_z * accel->accel_z)) * RAD_TO_DEG;

    float accel_roll = atan2f(
        accel->accel_y,
        accel->accel_z) * RAD_TO_DEG;

    // Complementary filter
    orientation.pitch =
        COMPLEMENTARY_ALPHA * gyro_pitch +
        (1.0f - COMPLEMENTARY_ALPHA) * accel_pitch;

    orientation.roll =
        COMPLEMENTARY_ALPHA * gyro_roll +
        (1.0f - COMPLEMENTARY_ALPHA) * accel_roll;
}

orientation_t state_estimator_get_orientation(void)
{
    return orientation;
}