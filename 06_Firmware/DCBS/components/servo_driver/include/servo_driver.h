#pragma once

//structs
typedef struct
{
    int gpio_pin;
    float current_angle;
    float target_angle;
    float min_pulse_us;
    float max_pulse_us;

} servo_t;


//functions
void servo_init(servo_t *servo);
void servo_set_target_angle(servo_t *servo, float angle);

//helper functions
int angle_to_pulse_us(servo_t *servo, float angle);
