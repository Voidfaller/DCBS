#pragma once

#include "esp_err.h"

esp_err_t watchdog_service_disable_temporarily(void);
esp_err_t watchdog_service_restore_default(void);
