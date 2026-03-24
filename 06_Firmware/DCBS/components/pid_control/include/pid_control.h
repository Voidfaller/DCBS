#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

typedef struct {
    float kp;
    float ki;
    float kd;

    float integral;
    float prev_measurement;

    float out_min;
    float out_max;
} pid_controller_t;

typedef struct {
    float pitch_output;
    float roll_output;
    float pitch_measurement;
    float roll_measurement;
} pid_output_t;

typedef struct {
    float pitch_measurement;
    float roll_measurement;
} pid_input_t;

extern QueueHandle_t pid_input_queue_handle;
extern QueueHandle_t pid_output_queue_handle;

void pid_init(pid_controller_t *pid,
                float kp,
                float ki,
                float kd,
                float out_min,
                float out_max);

void pid_reset(pid_controller_t *pid);

float pid_update (pid_controller_t *pid,
                 float setpoint_deg,
                 float measured_angle_deg,
                 float dt);

esp_err_t pid_control_start(void);
esp_err_t pid_control_reload_settings(void);
void pid_control_task(void *arg);
QueueHandle_t pid_control_get_queue_handle(void);
QueueHandle_t pid_control_get_input_queue_handle(void);
QueueHandle_t pid_control_get_monitor_queue_handle(void);

#endif /* PID_CONTROL_H */