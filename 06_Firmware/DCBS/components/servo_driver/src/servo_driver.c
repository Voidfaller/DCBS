#include "servo_driver.h"
#include "esp_err.h"
#include "driver/ledc.h"

// Initialize the servo
void servo_init(servo_t *servo)
{

    // Configure the LEDC timer
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50, // Standard servo frequency
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer_conf);

    // Configure the LEDC channel for the servo
    ledc_channel_config_t ch_conf = {
        .gpio_num = servo->gpio_pin,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ch_conf);

    int pulse = servo->min_pulse_us + (servo->current_angle / 180.0) * (servo->max_pulse_us - servo->min_pulse_us);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pulse);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}