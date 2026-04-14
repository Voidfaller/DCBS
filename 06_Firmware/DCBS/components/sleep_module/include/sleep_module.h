#pragma once

#include "esp_task_wdt.h"

typedef int sleep_timer_handle_t;

void sleep_timer_init(uint32_t hw_timeout_sec);
sleep_timer_handle_t sleep_timer_register(const char *name, uint32_t stay_awake_sec);
void sleep_timer_reset();
