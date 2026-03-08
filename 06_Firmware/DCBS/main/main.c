#include <stdio.h>
#include "servo_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main(void)
{
    printf("Hello, World!\n");
    servo_t servo = {
        .gpio_pin = 18,
        .current_angle = 0.0,
        .target_angle = 90.0,
        .min_pulse_us = 500.0,
        .max_pulse_us = 2500.0
    };
    servo_init(&servo);
}
