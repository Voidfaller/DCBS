#include "I2C_wrapper.h"

#include <string.h>

static i2c_master_bus_handle_t s_bus_handle = NULL;
static i2c_wrapper_config_t s_cfg;
static bool s_initialized = false;

static bool i2c_wrapper_is_valid_config(const i2c_wrapper_config_t *config)
{
	if (config == NULL)
	{
		return false;
	}
	if (config->port < 0 || config->port >= I2C_NUM_MAX)
	{
		return false;
	}
	if (config->scl_gpio < 0 || config->scl_gpio >= GPIO_NUM_MAX)
	{
		return false;
	}
	if (config->sda_gpio < 0 || config->sda_gpio >= GPIO_NUM_MAX)
	{
		return false;
	}
	if (config->scl_gpio == config->sda_gpio)
	{
		return false;
	}
	if (config->default_dev_speed_hz == 0U)
	{
		return false;
	}
	if (config->clk_src != I2C_CLK_SRC_DEFAULT && config->clk_src != I2C_CLK_SRC_APB)
	{
		return false;
	}
	return true;
}

esp_err_t i2c_wrapper_init(const i2c_wrapper_config_t *config)
{
	if (!i2c_wrapper_is_valid_config(config))
	{
		return ESP_ERR_INVALID_ARG;
	}
	if (s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}

	i2c_master_bus_config_t bus_cfg = {
		.clk_source = config->clk_src,
		.i2c_port = config->port,
		.scl_io_num = config->scl_gpio,
		.sda_io_num = config->sda_gpio,
		.glitch_ignore_cnt = config->glitch_ignore_cnt,
		.flags.enable_internal_pullup = config->enable_internal_pullup,
	};

	esp_err_t ret = i2c_new_master_bus(&bus_cfg, &s_bus_handle);
	if (ret != ESP_OK)
	{
		s_bus_handle = NULL;
		return ret;
	}

	s_cfg = *config;
	s_initialized = true;
	return ESP_OK;
}

bool i2c_wrapper_is_initialized(void)
{
	return s_initialized;
}

i2c_master_bus_handle_t i2c_wrapper_get_bus_handle(void)
{
	return s_bus_handle;
}

esp_err_t i2c_wrapper_add_device(uint8_t address_7bit,
								 uint32_t scl_speed_hz,
								 i2c_master_dev_handle_t *out_dev_handle)
{
	i2c_device_config_t dev_cfg;

	if (!s_initialized || out_dev_handle == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	if (address_7bit < 0x08 || address_7bit > 0x77)
	{
		return ESP_ERR_INVALID_ARG;
	}

	memset(&dev_cfg, 0, sizeof(dev_cfg));
	dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
	dev_cfg.device_address = address_7bit;
	dev_cfg.scl_speed_hz = (scl_speed_hz == 0U) ? s_cfg.default_dev_speed_hz : scl_speed_hz;

	return i2c_master_bus_add_device(s_bus_handle, &dev_cfg, out_dev_handle);
}

esp_err_t i2c_wrapper_scan(uint8_t *addresses,
						   size_t max_addresses,
						   size_t *found_count,
						   int timeout_ms)
{
	size_t found = 0;

	if (!s_initialized)
	{
		return ESP_ERR_INVALID_STATE;
	}

	for (uint8_t addr = 0x08; addr <= 0x77; addr++)
	{
		esp_err_t ret = i2c_master_probe(s_bus_handle, addr, timeout_ms);
		if (ret == ESP_OK)
		{
			if (addresses != NULL && found < max_addresses)
			{
				addresses[found] = addr;
			}
			found++;
		}
	}

	if (found_count != NULL)
	{
		*found_count = found;
	}

	return ESP_OK;
}

esp_err_t i2c_wrapper_txrx(i2c_master_dev_handle_t dev_handle,
						   const uint8_t *tx_data,
						   size_t tx_len,
						   uint8_t *rx_data,
						   size_t rx_len,
						   int timeout_ms)
{
	if (dev_handle == NULL || tx_data == NULL || tx_len == 0U || rx_data == NULL || rx_len == 0U)
	{
		return ESP_ERR_INVALID_ARG;
	}
	return i2c_master_transmit_receive(dev_handle, tx_data, tx_len, rx_data, rx_len, timeout_ms);
}

esp_err_t i2c_wrapper_tx(i2c_master_dev_handle_t dev_handle,
						 const uint8_t *tx_data,
						 size_t tx_len,
						 int timeout_ms)
{
	if (dev_handle == NULL || tx_data == NULL || tx_len == 0U)
	{
		return ESP_ERR_INVALID_ARG;
	}

	return i2c_master_transmit(dev_handle, tx_data, tx_len, timeout_ms);
}

esp_err_t i2c_wrapper_write_reg(i2c_master_dev_handle_t dev_handle,
								uint8_t reg,
								const uint8_t *data,
								size_t data_len,
								int timeout_ms)
{
	uint8_t buf[33];

	if (dev_handle == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}
	if (data_len > 32U)
	{
		return ESP_ERR_INVALID_SIZE;
	}
	if (data_len > 0U && data == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	buf[0] = reg;
	if (data_len > 0U)
	{
		memcpy(&buf[1], data, data_len);
	}

	return i2c_master_transmit(dev_handle, buf, data_len + 1U, timeout_ms);
}

esp_err_t i2c_wrapper_read_reg(i2c_master_dev_handle_t dev_handle,
							   uint8_t reg,
							   uint8_t *data,
							   size_t data_len,
							   int timeout_ms)
{
	if (dev_handle == NULL || data == NULL || data_len == 0U)
	{
		return ESP_ERR_INVALID_ARG;
	}

	return i2c_master_transmit_receive(dev_handle, &reg, 1, data, data_len, timeout_ms);
}

esp_err_t i2c_wrapper_write_reg_u8(i2c_master_dev_handle_t dev_handle,
								   uint8_t reg,
								   uint8_t value,
								   int timeout_ms)
{
	return i2c_wrapper_write_reg(dev_handle, reg, &value, 1, timeout_ms);
}

esp_err_t i2c_wrapper_read_reg_u8(i2c_master_dev_handle_t dev_handle,
								  uint8_t reg,
								  uint8_t *value,
								  int timeout_ms)
{
	return i2c_wrapper_read_reg(dev_handle, reg, value, 1, timeout_ms);
}

int i2c_wrapper_probe(uint8_t address_7bit, int timeout_ms)
{
	esp_err_t ret = i2c_master_probe(s_bus_handle, address_7bit, timeout_ms);
	if (ret == ESP_OK)
	{
		printf("Device found at I2C address 0x%02X\n", address_7bit);
	}
	else if (ret == ESP_ERR_TIMEOUT)
	{
		printf("No response from I2C address 0x%02X (timeout)\n", address_7bit);
	}
	else
	{
		printf("Error probing I2C address 0x%02X: 0x%x\n", address_7bit, ret);
	}
	return ret;
}