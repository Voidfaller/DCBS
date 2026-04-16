#pragma once

//libraries
#include <stdbool.h>
#include "gyro_driver.h"
#include "freertos/FreeRTOS.h"

//typedefs
typedef struct
{
    float pitch;
    float roll;
} orientation_t;

//globals
extern QueueHandle_t state_estimator_queue_handle;

//initialization and state
void state_estimator_init(void);

// main update function: fuses gyro and accel into stable orientaton
void state_estimator_update(
    imu_data_t *data,
    float dt);


// get current estimated orientation
orientation_t state_estimator_get_orientation(void);
QueueHandle_t state_estimator_get_queue_handle(void);
bool state_estimator_is_calibrated(void);
void state_estimator_recalibrate(void);

// main task loop: waits for IMU data, updates orientation estimate, and sends it to PID control task via queue
void state_estimator_task(void *arg);
esp_err_t state_estimator_start(void);
