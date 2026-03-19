#include <stdio.h>
#include "servo_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "I2C_wrapper.h"
void app_main(void)
{

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
    i2c_wrapper_init(&i2c_cfg);
    


    while (1)
    {

    }
}
