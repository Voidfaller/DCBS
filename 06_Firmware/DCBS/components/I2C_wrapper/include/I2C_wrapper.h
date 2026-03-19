#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_err.h"

typedef struct
{
	i2c_port_num_t port;
	gpio_num_t scl_gpio;
	gpio_num_t sda_gpio;
	uint32_t clk_src;
	uint8_t glitch_ignore_cnt;
	bool enable_internal_pullup;
	uint32_t default_dev_speed_hz;
} i2c_wrapper_config_t;

esp_err_t i2c_wrapper_init(const i2c_wrapper_config_t *config);

bool i2c_wrapper_is_initialized(void);
i2c_master_bus_handle_t i2c_wrapper_get_bus_handle(void);

esp_err_t i2c_wrapper_add_device(uint8_t address_7bit,
								 uint32_t scl_speed_hz,
								 i2c_master_dev_handle_t *out_dev_handle);
esp_err_t i2c_wrapper_probe(uint8_t address_7bit, int timeout_ms);
esp_err_t i2c_wrapper_scan(uint8_t *addresses,
						   size_t max_addresses,
						   size_t *found_count,
						   int timeout_ms);

esp_err_t i2c_wrapper_txrx(i2c_master_dev_handle_t dev_handle,
						   const uint8_t *tx_data,
						   size_t tx_len,
						   uint8_t *rx_data,
						   size_t rx_len,
						   int timeout_ms);

esp_err_t i2c_wrapper_write_reg(i2c_master_dev_handle_t dev_handle,
								uint8_t reg,
								const uint8_t *data,
								size_t data_len,
								int timeout_ms);
esp_err_t i2c_wrapper_read_reg(i2c_master_dev_handle_t dev_handle,
							   uint8_t reg,
							   uint8_t *data,
							   size_t data_len,
							   int timeout_ms);

esp_err_t i2c_wrapper_write_reg_u8(i2c_master_dev_handle_t dev_handle,
								   uint8_t reg,
								   uint8_t value,
								   int timeout_ms);
esp_err_t i2c_wrapper_read_reg_u8(i2c_master_dev_handle_t dev_handle,
								  uint8_t reg,
								  uint8_t *value,
								  int timeout_ms);

