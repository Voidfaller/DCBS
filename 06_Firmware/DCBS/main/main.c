#include <stdio.h>
#include "servo_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static bool send_servo_target(servo_t *servo, float angle, bool immediate)
{
    esp_err_t err = servo_set_target(servo, angle, immediate);
    if (err != ESP_OK)
    {
        printf("servo_set_target failed (angle=%.1f, immediate=%d, err=%d)\n", angle, immediate, (int)err);
        return false;
    }
    return true;
}

void app_main(void)
{
    printf("Servo demo start\n");



    //setup servo
    servo_t servo = {
        .gpio_pin = 18,
        .current_angle = 0.0,
        .target_angle = 0.0,
        .min_pulse_us = 1000.0,
        .max_pulse_us = 2000.0};

    servo_init(&servo);

    if (servo_start_task(&servo) != ESP_OK)
    {
        printf("Failed to start servo task\n");
        return;
    }

    while (1)
    {
    }
}
