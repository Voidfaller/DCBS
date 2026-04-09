#include "state_estimator.h"
#include <stdio.h>
#include <math.h>
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_timer.h"

#define RAD_TO_DEG 57.2958f
// Pid controller krijgt dit later pitch and roll
// bijvoorbeeld pitch = +5° → servo corrigeert
// roll = -3° → andere servo corrigeert
// complementary filter coefficient
#define COMPLEMENTARY_ALPHA 0.98f
#define STATE_ESTIMATOR_ZERO_CALIBRATION_SAMPLES 100U

static orientation_t orientation;
static TaskHandle_t s_state_estimator_task_handle = NULL;
QueueHandle_t state_estimator_queue_handle = NULL;
static float s_pitch_zero_offset_deg = 0.0f;
static float s_roll_zero_offset_deg = 0.0f;
static float s_pitch_zero_accum = 0.0f;
static float s_roll_zero_accum = 0.0f;
static uint32_t s_zero_sample_count = 0U;
static bool s_zero_calibrated = false;

static float s_gyro_x_bias = 0.0f;
static float s_gyro_y_bias = 0.0f;
static float s_gyro_z_bias = 0.0f;

static float s_gyro_x_accum = 0.0f;
static float s_gyro_y_accum = 0.0f;
static float s_gyro_z_accum = 0.0f;

static float compute_accel_pitch_deg(const imu_data_t *data)
{
    return atan2f(
               data->accel.accel_x,
               sqrtf(data->accel.accel_y * data->accel.accel_y +
                     data->accel.accel_z * data->accel.accel_z)) *
           RAD_TO_DEG;
}

static float compute_accel_roll_deg(const imu_data_t *data)
{
    return atan2f(
               data->accel.accel_y,
               data->accel.accel_z) *
           RAD_TO_DEG;
}

// Start altijd vlak (0)
void state_estimator_init(void)
{
    orientation.pitch = 0.0f;
    orientation.roll = 0.0f;
    s_pitch_zero_offset_deg = 0.0f;
    s_roll_zero_offset_deg = 0.0f;
    s_pitch_zero_accum = 0.0f;
    s_roll_zero_accum = 0.0f;
    s_zero_sample_count = 0U;
    s_zero_calibrated = false;
}

esp_err_t state_estimator_start(void)
{
    BaseType_t task_ok;

    if (s_state_estimator_task_handle != NULL)
    {
        return ESP_OK;
    }

    if (state_estimator_queue_handle == NULL)
    {
        state_estimator_queue_handle = xQueueCreate(1, sizeof(orientation_t));
        if (state_estimator_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
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

    // Subtract gyro bias
    float gx = data->gyro.gyro_x - s_gyro_x_bias;
    float gy = data->gyro.gyro_y - s_gyro_y_bias;
    float gz = data->gyro.gyro_z - s_gyro_z_bias;

    // nieuwe hoek = oude hoek + snelheid × tijd
    float gyro_pitch = orientation.pitch + data->gyro.gyro_y * dt;
    float gyro_roll = orientation.roll + data->gyro.gyro_x * dt;

    // Accelerometer angle calculation
    // Dit berekent de hoek t.o.v. zwaartekracht
    float accel_pitch = compute_accel_pitch_deg(data);
    float accel_roll = compute_accel_roll_deg(data);

    // Apply the zero calibration to the measurement, not to the fused state.
    accel_pitch -= s_pitch_zero_offset_deg;
    accel_roll -= s_roll_zero_offset_deg;

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

QueueHandle_t state_estimator_get_queue_handle(void)
{
    return state_estimator_queue_handle;
}

void state_estimator_task(void *arg)
{
    (void)arg;
    uint32_t trace_div = 0U;
    uint64_t last_time_us = esp_timer_get_time();
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
            uint64_t now_us = esp_timer_get_time();
            float dt = (now_us - last_time_us) / 1000000.0f;
            last_time_us = now_us;

            if (!s_zero_calibrated)
            {
                s_pitch_zero_accum += compute_accel_pitch_deg(&data);
                s_roll_zero_accum += compute_accel_roll_deg(&data);

                // Accumulate samples for gyro bias estimation
                s_gyro_x_accum += data.gyro.gyro_x;
                s_gyro_y_accum += data.gyro.gyro_y;
                s_gyro_z_accum += data.gyro.gyro_z;

                s_zero_sample_count++;

                if (s_zero_sample_count >= STATE_ESTIMATOR_ZERO_CALIBRATION_SAMPLES)
                {
                    s_pitch_zero_offset_deg = s_pitch_zero_accum / (float)s_zero_sample_count;
                    s_roll_zero_offset_deg = s_roll_zero_accum / (float)s_zero_sample_count;

                    s_gyro_x_bias = s_gyro_x_accum / (float)s_zero_sample_count;
                    s_gyro_y_bias = s_gyro_y_accum / (float)s_zero_sample_count;
                    s_gyro_z_bias = s_gyro_z_accum / (float)s_zero_sample_count;

                    s_zero_calibrated = true;
                    orientation.pitch = 0.0f;
                    orientation.roll = 0.0f;
                    printf("State estimator zero calibrated: pitch=%.2f roll=%.2f\n",
                           (double)s_pitch_zero_offset_deg,
                           (double)s_roll_zero_offset_deg);

                    printf("Gyro biases: gx=%.3f gy=%.3f gz=%.3f\n",
                           (double)s_gyro_x_bias,
                           (double)s_gyro_y_bias,
                           (double)s_gyro_z_bias);
                }
            }

            state_estimator_update(&data, dt); // 10ms update interval
            if (state_estimator_queue_handle != NULL)
            {
                orientation_t current_orientation = state_estimator_get_orientation();
                xQueueOverwrite(state_estimator_queue_handle, &current_orientation);
                // printf("Orientation updated: pitch=%.2f, roll=%.2f\n", current_orientation.pitch, current_orientation.roll);

                trace_div++;
                if (trace_div >= 20U)
                {
                    printf("[estimate] pitch=%.2f roll=%.2f\n",
                           current_orientation.pitch,
                           current_orientation.roll);
                    trace_div = 0U;
                }
            }
        }
    }
}