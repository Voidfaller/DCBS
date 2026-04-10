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
    uint64_t last_time_us;
} pid_controller_t;

QueueHandle_t pid_control_get_queue_handle(void);
void pid_controller_init(pid_controller_t *pid);
float pid_controller_update(pid_controller_t *pid, orientation_t measurement);
esp_err_t pid_control_start(void);

