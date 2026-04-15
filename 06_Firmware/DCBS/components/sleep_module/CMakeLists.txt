idf_component_register(
    SRCS "sleep_module.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos driver esp_timer
    )
