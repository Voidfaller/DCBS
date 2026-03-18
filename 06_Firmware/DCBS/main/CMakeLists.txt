idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES servo_driver gyro_driver config_manager log)
