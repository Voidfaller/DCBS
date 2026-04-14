#include "config_manager.h"

#include <string.h>

#include "nvs.h"
#include "nvs_flash.h"

#define CONFIG_NAMESPACE "app_cfg"
#define CONFIG_KEY "settings"
#define CONFIG_MAGIC 0x4346474DU
#define CONFIG_VERSION 2U

typedef struct
{
	uint32_t magic;
	uint16_t version;
	uint16_t reserved;
	app_config_t values;
} stored_config_blob_t;

static const app_config_t k_default_config = {
	.pid_setpoint = 0,
	.pid_kp = 20,
	.pid_ki = 0,
	.pid_kd = 0,
	.servo_min_us = 500,
	.servo_max_us = 2500,
	.ui_frame_ms = 50,
	.pitch_setpoint_deg = 0.0f,
	.roll_setpoint_deg = 0.0f,
};

static nvs_handle_t s_nvs_handle = 0;
static bool s_initialized = false;
static app_config_t s_config;

static esp_err_t config_manager_read_blob(stored_config_blob_t *out_blob)
{
	size_t blob_size = sizeof(*out_blob);
	esp_err_t ret;

	if (out_blob == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	ret = nvs_get_blob(s_nvs_handle, CONFIG_KEY, out_blob, &blob_size);
	if (ret != ESP_OK)
	{
		return ret;
	}

	if (blob_size != sizeof(*out_blob) || out_blob->magic != CONFIG_MAGIC || out_blob->version != CONFIG_VERSION)
	{
		return ESP_ERR_INVALID_VERSION;
	}

	return ESP_OK;
}

static esp_err_t config_manager_write_blob(void)
{
	stored_config_blob_t blob = {
		.magic = CONFIG_MAGIC,
		.version = CONFIG_VERSION,
		.reserved = 0,
		.values = s_config,
	};
	esp_err_t ret = nvs_set_blob(s_nvs_handle, CONFIG_KEY, &blob, sizeof(blob));

	if (ret != ESP_OK)
	{
		return ret;
	}

	return nvs_commit(s_nvs_handle);
}

static void config_manager_load_defaults(void)
{
	s_config = k_default_config;
}

esp_err_t config_manager_init(void)
{
	esp_err_t ret;
	stored_config_blob_t blob;

	if (s_initialized)
	{
		return ESP_OK;
	}

	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	if (ret != ESP_OK)
	{
		return ret;
	}

	ret = nvs_open(CONFIG_NAMESPACE, NVS_READWRITE, &s_nvs_handle);
	if (ret != ESP_OK)
	{
		return ret;
	}

	ret = config_manager_read_blob(&blob);
	if (ret == ESP_ERR_NVS_NOT_FOUND)
	{
		config_manager_load_defaults();
		ret = config_manager_write_blob();
		if (ret != ESP_OK)
		{
			return ret;
		}
	}
	else if (ret == ESP_OK)
	{
		s_config = blob.values;
	}
	else
	{
		config_manager_load_defaults();
		ret = config_manager_write_blob();
		if (ret != ESP_OK)
		{
			return ret;
		}
	}

	s_initialized = true;
	return ESP_OK;
}

bool config_manager_is_initialized(void)
{
	return s_initialized;
}

const app_config_t *config_manager_get(void)
{
	return &s_config;
}

void config_manager_get_copy(app_config_t *out_config)
{
	if (out_config == NULL)
	{
		return;
	}
	*out_config = s_config;
}
// NEW: Get current setpoints and also set new setpoints in the config manager on touch events 
esp_err_t config_manager_get_current_setpoints(float *pitch_setpoint_deg, float *roll_setpoint_deg)
{
	if (pitch_setpoint_deg == NULL || roll_setpoint_deg == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	*pitch_setpoint_deg = s_config.pitch_setpoint_deg;
	*roll_setpoint_deg = s_config.roll_setpoint_deg;
	return ESP_OK;
}

esp_err_t config_manager_set_current_setpoints(float pitch_setpoint_deg,
								   float roll_setpoint_deg,
								   bool persist_immediately)
{
	if (!s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}

	s_config.pitch_setpoint_deg = pitch_setpoint_deg;
	s_config.roll_setpoint_deg = roll_setpoint_deg;

	if (persist_immediately)
	{
		return config_manager_write_blob();
	}

	return ESP_OK;
}

esp_err_t config_manager_save(void)
{
	if (!s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}

	return config_manager_write_blob();
}

esp_err_t config_manager_reload(void)
{
	stored_config_blob_t blob;
	esp_err_t ret;

	if (!s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}

	ret = config_manager_read_blob(&blob);
	if (ret != ESP_OK)
	{
		return ret;
	}

	s_config = blob.values;
	return ESP_OK;
}

esp_err_t config_manager_update(const app_config_t *new_config, bool persist_immediately)
{
	if (!s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}
	if (new_config == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	s_config = *new_config;

	if (persist_immediately)
	{
		return config_manager_write_blob();
	}

	return ESP_OK;
}

esp_err_t config_manager_reset_to_defaults(bool persist_immediately)
{
	if (!s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}

	config_manager_load_defaults();

	if (persist_immediately)
	{
		return config_manager_write_blob();
	}

	return ESP_OK;
}