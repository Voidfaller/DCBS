idf_component_register(
    SRCS "src/touch_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES driver freertos esp_hw_support esp_timer
)
