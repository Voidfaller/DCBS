#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "config_manager.h"
#include "I2C_wrapper.h"
#include "display_driver.h"

void app_main(void)
{

    // Initialize configuration manager and load config
    const app_config_t *cfg;
    ESP_ERROR_CHECK(config_manager_init());
    cfg = config_manager_get();
    (void)cfg;

    // Initialize I2C wrapper
    i2c_wrapper_config_t i2c_cfg = {
        .port = I2C_NUM_0,
        .scl_gpio = GPIO_NUM_22,
        .sda_gpio = GPIO_NUM_21,
        .clk_src = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 0,
        .enable_internal_pullup = true,
        .default_dev_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_wrapper_init(&i2c_cfg));

    // Initialize display driver and start UI task
    const display_ui_setting_t ui_settings[] = {
        DISPLAY_UI_SETTING("Setpoint", &cfg->pid_setpoint, -1000, 1000, 10),
        DISPLAY_UI_SETTING("Kp", &cfg->pid_kp, 0, 1000, 1),
        DISPLAY_UI_SETTING("Ki", &cfg->pid_ki, 0, 1000, 1),
        DISPLAY_UI_SETTING("Kd", &cfg->pid_kd, 0, 1000, 1),
    };

    ESP_ERROR_CHECK(display_ui_start_task(0x3C, 128, 64, ui_settings, sizeof(ui_settings) / sizeof(ui_settings[0])));


    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
