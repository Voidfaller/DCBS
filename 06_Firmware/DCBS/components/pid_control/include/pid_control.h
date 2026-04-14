#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "state_estimator.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    float setpoint;
    float integral;
    float last_error;
    float filtered_derivative;
    uint64_t last_time_us;
} pid_controller_t;


typedef struct 
{
    float pitch_pwm;
    float roll_pwm;
} pid_correction_t;


QueueHandle_t pid_control_get_queue_handle(void);
void pid_controller_init(void);
float pid_controller_update(pid_controller_t *pid, float measurement);
esp_err_t pid_control_start(void);

