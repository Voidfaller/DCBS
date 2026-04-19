#include "state_estimator.h"
#include <stdio.h>
#include <math.h>
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_timer.h"

#define RAD_TO_DEG 57.2958f                           // conversion factor from radians to degrees
#define COMPLEMENTARY_ALPHA 0.98f                     // complementary filter coefficient: gyro vs accel blending
#define STATE_ESTIMATOR_ZERO_CALIBRATION_SAMPLES 100U // number of samples used for zero callibration as starting point

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

// compute pitch angle from accelerometer data
// used gravity vector projection
static float compute_accel_pitch_deg(const imu_data_t *data)
{
    return atan2f(
               -data->accel.accel_x,
               sqrtf(data->accel.accel_y * data->accel.accel_y +
                     data->accel.accel_z * data->accel.accel_z)) *
           RAD_TO_DEG;
}
// compute roll angle from accelerometer data
static float compute_accel_roll_deg(const imu_data_t *data)
{
    return atan2f(
               data->accel.accel_y,
               data->accel.accel_z) *
           RAD_TO_DEG;
}

// initialize estimator state and reset calibration
// assumes system starts in level position
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
// start estimator task and create queue
esp_err_t state_estimator_start(void)
{
    BaseType_t task_ok;
    // prevent multiple task creation
    if (s_state_estimator_task_handle != NULL)
    {
        return ESP_OK;
    }
    // create queue for orientation output
    if (state_estimator_queue_handle == NULL)
    {
        state_estimator_queue_handle = xQueueCreate(1, sizeof(orientation_t));
        if (state_estimator_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }
    // Initialize estimator state
    state_estimator_init();
    // create Freetos task for state estimation loop
    task_ok = xTaskCreate(state_estimator_task, "state_estimator_task", 2048, NULL, 10, &s_state_estimator_task_handle);
    if (task_ok != pdPASS)
    {
        s_state_estimator_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}
// Main update function: fuses gyro and accel into stable orientaton
void state_estimator_update(imu_data_t *data, float dt)
{
    // remove gryo bias
    float gx = data->gyro.gyro_x - s_gyro_x_bias;
    float gy = data->gyro.gyro_y - s_gyro_y_bias;
    // integrates gyro rates to estimate angle
    float gyro_pitch = orientation.pitch + gy * dt;
    float gyro_roll = orientation.roll + gx * dt;
    // compute angles from acceleromter

    float accel_pitch = compute_accel_pitch_deg(data);
    float accel_roll = compute_accel_roll_deg(data);

    // apply zero offset calibration
    accel_pitch -= s_pitch_zero_offset_deg;
    accel_roll -= s_roll_zero_offset_deg;

    // Complementary filter
    // gyro: fast response but drifts over time
    // accel: stable long term, but noisy
    orientation.pitch =
        COMPLEMENTARY_ALPHA * gyro_pitch +
        (1.0f - COMPLEMENTARY_ALPHA) * accel_pitch;

    orientation.roll =
        COMPLEMENTARY_ALPHA * gyro_roll +
        (1.0f - COMPLEMENTARY_ALPHA) * accel_roll;
}
// get current estimated orientation
orientation_t state_estimator_get_orientation(void)
{
    return orientation;
}
// get queue handle for other modules
QueueHandle_t state_estimator_get_queue_handle(void)
{
    return state_estimator_queue_handle;
}

// check if zero calibration is complete
bool state_estimator_is_calibrated(void)
{
    return s_zero_calibrated;
}

// reset calibration state to trigger recalibration on next samples
void state_estimator_recalibrate(void)
{
    s_zero_calibrated = false;
    s_zero_sample_count = 0U;
    s_pitch_zero_accum = 0.0f;
    s_roll_zero_accum = 0.0f;
    s_gyro_x_accum = 0.0f;
    s_gyro_y_accum = 0.0f;
    s_gyro_z_accum = 0.0f;
    printf("[estimator] Recalibration triggered, collecting samples...\n");
}

// task loop: waits reads IMU data, updates orientation and publishes it
void state_estimator_task(void *arg)
{
    (void)arg;
    // timestamp for dt calculation
    uint64_t last_time_us = esp_timer_get_time();
    while (1)
    {
        // wait until gyro driver queue is available
        if (gyro_queue_handle == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        imu_data_t data;
        // revieve IMU data gyro and accel
        if (xQueueReceive(gyro_queue_handle, &data, pdMS_TO_TICKS(100)) == pdPASS)
        {
            uint64_t now_us = esp_timer_get_time();
            float dt = (now_us - last_time_us) / 1000000.0f;
            last_time_us = now_us;

            if (!s_zero_calibrated) // calibration phase: determine zero offsets and gyro bias
            {
                // accumulate accelerometer angles
                s_pitch_zero_accum += compute_accel_pitch_deg(&data);
                s_roll_zero_accum += compute_accel_roll_deg(&data);

                // Accumulate samples for gyro bias estimation
                s_gyro_x_accum += data.gyro.gyro_x;
                s_gyro_y_accum += data.gyro.gyro_y;
                s_gyro_z_accum += data.gyro.gyro_z;

                s_zero_sample_count++;
                // once enough samples are collected, compute averages
                if (s_zero_sample_count >= STATE_ESTIMATOR_ZERO_CALIBRATION_SAMPLES)
                {
                    s_pitch_zero_offset_deg = s_pitch_zero_accum / (float)s_zero_sample_count;
                    s_roll_zero_offset_deg = s_roll_zero_accum / (float)s_zero_sample_count;

                    s_gyro_x_bias = s_gyro_x_accum / (float)s_zero_sample_count;
                    s_gyro_y_bias = s_gyro_y_accum / (float)s_zero_sample_count;
                    s_gyro_z_bias = s_gyro_z_accum / (float)s_zero_sample_count;

                    s_zero_calibrated = true;
                    // reset orientation after calibration to ensure it starts from the calibrated zero position
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

            state_estimator_update(&data, dt); // update orientation estimate
            if (state_estimator_queue_handle != NULL)
            {
                orientation_t current_orientation = state_estimator_get_orientation(); // send latest oriention to queue
                xQueueOverwrite(state_estimator_queue_handle, &current_orientation);
            }
        }
    }
}