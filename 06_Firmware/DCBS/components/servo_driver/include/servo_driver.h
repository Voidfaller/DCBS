#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/ledc.h"

//structs
typedef struct
{
    int gpio_pin;
    ledc_mode_t speed_mode;
    ledc_timer_t timer_num;
    ledc_channel_t channel;
    float current_angle;
    float target_angle;
    float min_angle_deg;
    float max_angle_deg;
    float min_pulse_us;
    float max_pulse_us;
    uint32_t task_period_ms;
    float task_step_deg;
    uint32_t cmd_queue_len;
    uint32_t task_stack_size;
    uint32_t task_priority;
    void *cmd_queue_handle;
    void *task_handle;

} servo_t;

typedef struct
{
    float target_angle;
    bool immediate;
} servo_cmd_t;

//functions
void servo_init(servo_t *servo);
void servo_rotate(servo_t *servo, float angle);
esp_err_t servo_start_task(servo_t *servo);
esp_err_t servo_send_command(servo_t *servo, const servo_cmd_t *cmd);
esp_err_t servo_set_target(servo_t *servo, float angle, bool immediate);
esp_err_t servo_stabilizer_start_pitch(void);
void servo_set_pitch_pid_bypass(bool enabled);
bool servo_get_pitch_pid_bypass(void);
//helper functions
int angle_to_pulse_us(servo_t *servo, float angle);
