idf_component_register(
    SRCS "src/display_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos driver esp_timer heap I2C_wrapper config_manager gyro_driver pid_control servo_driver watchdog_service
)