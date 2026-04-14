idf_component_register(
    SRCS "src/display_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2c esp_driver_gpio I2C_wrapper config_manager gyro_driver pid_control servo_driver state_estimator
    )