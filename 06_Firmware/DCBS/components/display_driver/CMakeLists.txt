idf_component_register(
    SRCS "src/display_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2c esp_driver_gpio freertos I2C_wrapper esp_timer config_manager
    )