idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos I2C_wrapper display_driver esp_driver_i2c esp_driver_gpio servo_driver)
