#pragma once

#include "freetos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include <stdbool.h>



typedef enum {
    ENERGY_EVENT_ACTIVITY,
    ENERGY_EVENT_FORCE_SLEEP

} energy_event_t;

typedef int energy_handle_t;

esp_err_t energy_manager_init(uint32_t timeout_sec); // init system
energy_handle_t energy_manager_register(const char *name);
void energy_manager_feed(energy_handle_t handle);
QueueHandle_t energy_manager_get_queue(void);
void energy_manager_check_wakeup(void); // call in app_main