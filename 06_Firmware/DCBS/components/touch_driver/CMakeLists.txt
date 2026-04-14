idf_component_register(
    SRCS "touch_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES driver freertos config_manager state_estimator 
)