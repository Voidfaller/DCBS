#include "watchdog_service.h"

#include <stdbool.h>

#include "esp_task_wdt.h"
#include "sdkconfig.h"

static bool s_task_wdt_temporarily_disabled = false;

esp_err_t watchdog_service_disable_temporarily(void)
{
	esp_err_t err;

	if (s_task_wdt_temporarily_disabled)
	{
		return ESP_OK;
	}

	err = esp_task_wdt_deinit();
	if (err == ESP_ERR_INVALID_STATE)
	{
		return ESP_OK;
	}
	if (err != ESP_OK)
	{
		return err;
	}

	s_task_wdt_temporarily_disabled = true;
	return ESP_OK;
}

esp_err_t watchdog_service_restore_default(void)
{
#ifdef CONFIG_ESP_TASK_WDT_PANIC
	const bool trigger_panic = true;
#else
	const bool trigger_panic = false;
#endif

	esp_task_wdt_config_t cfg = {
		.timeout_ms = CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000,
		.idle_core_mask = 0,
		.trigger_panic = trigger_panic,
	};

	if (!s_task_wdt_temporarily_disabled)
	{
		return ESP_OK;
	}

#if CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
	cfg.idle_core_mask |= (1U << 0);
#endif
#if CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
	cfg.idle_core_mask |= (1U << 1);
#endif

	esp_err_t err = esp_task_wdt_init(&cfg);
	if (err != ESP_OK)
	{
		return err;
	}

	s_task_wdt_temporarily_disabled = false;
	return ESP_OK;
}
