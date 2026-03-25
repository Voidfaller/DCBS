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

#define DISABLE_TASK_WDT_TEMPORARY 1

void app_main(void)
{
    esp_err_t err;

    config_manager_init();

#if DISABLE_TASK_WDT_TEMPORARY
    err = watchdog_service_disable_temporarily();
    if (err != ESP_OK)
    {
        printf("Failed to disable Task WDT temporarily: %s\n", esp_err_to_name(err));
        return;
    }
    printf("Task WDT temporarily disabled\n");
#endif

    i2c_wrapper_config_t i2c_cfg = {
        .port = I2C_NUM_0,
        .scl_gpio = 22,
        .sda_gpio = 21,
        .clk_src = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .enable_internal_pullup = true,
        .default_dev_speed_hz = 400000,
    };

    err = i2c_wrapper_init(&i2c_cfg);
    if (err != ESP_OK)
    {
        printf("I2C wrapper init failed: %s\n", esp_err_to_name(err));
        return;
    }

    err = gyro_driver_start();
    if (err != ESP_OK)
    {
        printf("Failed to start gyro driver: %s\n", esp_err_to_name(err));
        return;
    }

    err = state_estimator_start();
    if (err != ESP_OK)
    {
        printf("Failed to start state estimator: %s\n", esp_err_to_name(err));
        return;
    }

    //config manager update test
    app_config_t cfg;
    config_manager_get_copy(&cfg);

    cfg.pid_kp = 20;
    config_manager_update(&cfg, true);


    err = pid_control_start();
    if (err != ESP_OK)
    {
        printf("Failed to start pid control: %s\n", esp_err_to_name(err));
        return;
    }

    err = servo_stabilizer_start_pitch();
    if (err != ESP_OK)
    {
        printf("Failed to start pitch servo control: %s\n", esp_err_to_name(err));
        return;
    }

    err = display_driver_start();
    if (err != ESP_OK)
    {
        printf("Failed to start display driver: %s\n", esp_err_to_name(err));
        return;
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
