idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES freertos config_manager I2C_wrapper gyro_driver state_estimator pid_control servo_driver watchdog_service display_driver touch_driver sleep_module)
