#pragma once

#include "esp_task_wdt.h"

typedef int watchdog_handle_t;

void watchdog_init(uint32_t hw_timeout_sec);
watchdog_handle_t watchdog_register(const char *name, uint32_t timeout_sec);
void watchdog_feed(watchdog_handle_t handle);