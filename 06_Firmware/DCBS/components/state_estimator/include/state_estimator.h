#pragma once

#include "esp_err.h"

typedef struct
{
    float roll;
    float pitch;
} state_estimate_t;

esp_err_t state_estimator_init(void);

esp_err_t state_estimator_update(float dt);

state_estimate_t state_estimator_get_state(void);