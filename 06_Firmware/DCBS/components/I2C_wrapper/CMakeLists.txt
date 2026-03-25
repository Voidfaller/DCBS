idf_component_register(
    SRCS "src/I2C_wrapper.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2c esp_driver_gpio
    )