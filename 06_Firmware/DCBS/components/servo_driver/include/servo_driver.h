#pragma once

#include <stdbool.h>
#include "esp_err.h"

//structs
typedef struct
{
    int gpio_pin;
    float current_angle;
    float target_angle;
    float min_pulse_us;
    float max_pulse_us;

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
//helper functions
int angle_to_pulse_us(servo_t *servo, float angle);
