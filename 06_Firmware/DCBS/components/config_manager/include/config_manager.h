#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

typedef struct
{
	int32_t pid_setpoint;
	int32_t pid_kp;
	int32_t pid_ki;
	int32_t pid_kd;
	int32_t servo_min_us;
	int32_t servo_max_us;
	uint32_t ui_frame_ms;
	float pitch_setpoint_deg;
	float roll_setpoint_deg;
} app_config_t;

esp_err_t config_manager_init(void);
bool config_manager_is_initialized(void);

const app_config_t *config_manager_get(void);
void config_manager_get_copy(app_config_t *out_config);
esp_err_t config_manager_set_current_setpoints(float pitch_setpoint_deg,
								   float roll_setpoint_deg,
								   bool persist_immediately);
esp_err_t config_manager_get_current_setpoints(float *pitch_setpoint_deg,
								   float *roll_setpoint_deg);

esp_err_t config_manager_save(void);
esp_err_t config_manager_reload(void);
esp_err_t config_manager_update(const app_config_t *new_config, bool persist_immediately);
esp_err_t config_manager_reset_to_defaults(bool persist_immediately);
