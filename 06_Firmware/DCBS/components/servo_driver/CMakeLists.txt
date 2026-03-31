idf_component_register(
    SRCS "src/servo_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos pid_control esp_driver_ledc
)