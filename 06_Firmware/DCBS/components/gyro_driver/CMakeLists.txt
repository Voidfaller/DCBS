idf_component_register(
    SRCS "src/gyro_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES driver I2C_wrapper esp_timer watchdog_service
)