#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifndef TOUCH_PIN
#define TOUCH_PIN TOUCH_PAD_NUM0     // REVIEW: Dit is op de ESP32S NodeMCU, maar check of dit klopt voor de gebruikte board
#endif

#ifndef TOUCH_THRESHOLD
#define TOUCH_THRESHOLD 70U         // REVIEW: check of deze threshold goed is later
#endif

#ifndef TOUCH_POLL_MS
#define TOUCH_POLL_MS 200U
#endif

typedef struct
{
    uint32_t touch_value;
    bool touched;

} touch_data_t;

esp_err_t touch_driver_init(void);
esp_err_t touch_driver_start(void);
esp_err_t touch_driver_read(touch_data_t *data);
void touch_driver_task(void *pvParameters);


