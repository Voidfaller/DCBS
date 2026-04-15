#pragma once

#include "sleep_module.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_log.h"

void sleep_timer_reset(uint32_t time_until_sleep_sec);
void sleep_start();
esp_err_t sleep_timer_init(uint32_t time_until_sleep_sec);
