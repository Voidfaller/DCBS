#pragma once

//libraries
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/touch_pad.h"

//configurable parameters with defaults
#ifndef TOUCH_PIN
#define TOUCH_PIN TOUCH_PAD_NUM3
#endif

#ifndef TOUCH_THRESHOLD
#define TOUCH_THRESHOLD 160U
#endif

#ifndef TOUCH_SLEEP_TIMEOUT_MS
#define TOUCH_SLEEP_TIMEOUT_MS 3000U
#endif


//typedefs
typedef struct
{
    uint32_t touch_value;
    bool touched;
} touch_data_t;

//initialization and state
esp_err_t touch_driver_init(void);
esp_err_t touch_driver_start(void);
esp_err_t touch_driver_read(touch_data_t *data);
void touch_driver_task(void *pvParameters);
