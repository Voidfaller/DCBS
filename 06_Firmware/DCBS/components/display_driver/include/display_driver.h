#pragma once

#include <stdint.h>

#include "esp_err.h"
#include "driver/i2c_master.h"

typedef struct
{
    int SCL_GPIO;
    int SDA_GPIO;
    uint8_t addr;
    i2c_master_dev_handle_t dev_handle;
} display_t;

void display_init(display_t *display, i2c_master_bus_handle_t bus_handle, uint8_t addr);
void display_test(display_t *display);