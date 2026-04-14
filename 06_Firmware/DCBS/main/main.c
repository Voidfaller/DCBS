#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "config_manager.h"
#include "I2C_wrapper.h"
#include "gyro_driver.h"
#include "state_estimator.h"
#include "pid_control.h"
#include "servo_driver.h"
#include "watchdog_service.h"
#include "display_driver.h"
#include "esp_timer.h"

void app_main(void)
{

    // // Initialize the watchdog with a timeout of 5 seconds
    // watchdog_init(5);
    esp_err_t err;

    // printf("[startup] app_main begin\n");

    // Initialize the config manager first to ensure settings are available for other components
    err = config_manager_init();
    if (err != ESP_OK)
    {
        printf("[startup] config_manager_init failed: %s\n", esp_err_to_name(err));
        return;
    }
    printf("[startup] config manager ready\n");

    // Boot test profile: force a visible movement command and keep SG90 in a safe pulse range.
    app_config_t cfg;
    config_manager_get_copy(&cfg);
    cfg.pid_setpoint = 15;
    cfg.servo_min_us = 1000;
    cfg.servo_max_us = 2000;
    err = config_manager_update(&cfg, false);
    if (err != ESP_OK)
    {
        printf("[startup] config_manager_update failed: %s\n", esp_err_to_name(err));
        return;
    }

    // Initialize the I2C wrapper before any components that depend on I2C
    i2c_wrapper_config_t i2c_cfg = {
        .port = I2C_NUM_0,
        .scl_gpio = 22,
        .sda_gpio = 21,
        .clk_src = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .enable_internal_pullup = true,
        .default_dev_speed_hz = 400000,
    };

    err = i2c_wrapper_init(&i2c_cfg); // initialize the I2C wrapper with the specified config
    if (err != ESP_OK)
    {
        printf("I2C wrapper init failed: %s\n", esp_err_to_name(err));
        return;
    }
    printf("[startup] i2c wrapper ready\n");

    err = display_driver_start(); // start the display driver task
    if (err != ESP_OK)
    {
        printf("Failed to start display driver: %s\n", esp_err_to_name(err));
        return;
    }
    printf("[startup] display driver ready\n");

    err = gyro_driver_start(); // start the gyro driver task
    if (err != ESP_OK)
    {
        printf("Failed to start gyro driver: %s\n", esp_err_to_name(err));
        return;
    }
    printf("[startup] gyro driver ready\n");

    err = state_estimator_start(); // start the state estimator task
    if (err != ESP_OK)
    {
        printf("Failed to start state estimator: %s\n", esp_err_to_name(err));
        return;
    }
    printf("[startup] state estimator ready\n");

    err = pid_control_start(); // start the PID control task 
    if (err != ESP_OK)
    {
        printf("Failed to start pid control: %s\n", esp_err_to_name(err));
        return;
    }
    printf("[startup] pid control ready\n");

    err = servo_start_task(false); // true = bypass PID, use state estimator directly

    if (err != ESP_OK)
    {
        printf("Failed to start pitch servo control: %s\n", esp_err_to_name(err));
        // watchdog_register("Servo", 5);
        return;
    }
    printf("[startup]  servo's ready\n");

    // // Use PID mode so configured setpoint drives the servo even when board is level.

    // printf("[startup] system ready\n");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}
