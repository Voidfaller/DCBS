#include "state_estimator.h"
#include <stdio.h>
#include <math.h>

#define RAD_TO_DEG 57.2958f
// Pid controller krijgt dit later pitch and roll
// bijvoorbeeld pitch = +5° → servo corrigeert
// roll = -3° → andere servo corrigeert
// complementary filter coefficient
#define COMPLEMENTARY_ALPHA 0.98f

static orientation_t orientation;
static TaskHandle_t s_state_estimator_task_handle = NULL;

//Start altijd vlak (0) 
void state_estimator_init(void)
{
    orientation.pitch = 0.0f;
    orientation.roll = 0.0f;
}

esp_err_t state_estimator_start(void)
{
    BaseType_t task_ok;

    if (s_state_estimator_task_handle != NULL)
    {
        return ESP_OK;
    }

    state_estimator_init();

    task_ok = xTaskCreate(state_estimator_task, "state_estimator_task", 2048, NULL, 10, &s_state_estimator_task_handle);
    if (task_ok != pdPASS)
    {
        s_state_estimator_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}
void state_estimator_update(imu_data_t *data, float dt)
{
    // Gyroscope integration
    // nieuwe hoek = oude hoek + snelheid × tijd
    float gyro_pitch = orientation.pitch + data->gyro.gyro_y * dt;
    float gyro_roll  = orientation.roll  + data->gyro.gyro_x * dt;

    // Accelerometer angle calculation
    // Dit berekent de hoek t.o.v. zwaartekracht
    float accel_pitch = atan2f(
        data->accel.accel_x,
        sqrtf(data->accel.accel_y * data->accel.accel_y +
              data->accel.accel_z * data->accel.accel_z)) * RAD_TO_DEG;

    float accel_roll = atan2f(
        data->accel.accel_y,
        data->accel.accel_z) * RAD_TO_DEG;

    // Complementary filter
    orientation.pitch =
        COMPLEMENTARY_ALPHA * gyro_pitch +
        (1.0f - COMPLEMENTARY_ALPHA) * accel_pitch;

    orientation.roll =
        COMPLEMENTARY_ALPHA * gyro_roll +
        (1.0f - COMPLEMENTARY_ALPHA) * accel_roll;
}

// return de huidige geschatte orientatie: PID_controller heeft dit nodig 
orientation_t state_estimator_get_orientation(void)
{
    return orientation;
}

void state_estimator_task(void *arg)
{
    (void)arg;
    while (1)
    {
        if (gyro_queue_handle == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        imu_data_t data;
        if (xQueueReceive(gyro_queue_handle, &data, pdMS_TO_TICKS(100)) == pdPASS)
        {
            state_estimator_update(&data, 0.01f); // 10ms update interval
        
            //debug print
            orientation_t current_orientation = state_estimator_get_orientation();
            printf("Pitch: %.2f, Roll: %.2f\n", current_orientation.pitch, current_orientation.roll);
        }
    }
}   