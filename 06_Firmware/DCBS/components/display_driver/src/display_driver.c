#include "display_driver.h"
#include <freertos/FreeRTOS.h>
#include <string.h>

static void ssd1306_send_cmd(display_t *display, uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd}; // Co=0, D/C#=0 (command)
    i2c_master_transmit(display->dev_handle, buf, 2, pdMS_TO_TICKS(100));
}

static void ssd1306_send_data(display_t *display, const uint8_t *data, size_t len)
{
    // Send in chunks of 32 bytes to avoid I2C timeout
    size_t offset = 0;
    while (offset < len) {
        size_t chunk = (len - offset > 32) ? 32 : (len - offset);
        uint8_t buf[33];
        buf[0] = 0x40; // Co=0, D/C#=1 (data)
        memcpy(&buf[1], data + offset, chunk);
        i2c_master_transmit(display->dev_handle, buf, chunk + 1, pdMS_TO_TICKS(100));
        offset += chunk;
    }
}

static void ssd1306_reset_cursor(display_t *display)
{
    ssd1306_send_cmd(display, 0x21); // set column address
    ssd1306_send_cmd(display, 0);    // start column
    ssd1306_send_cmd(display, 127);  // end column
    ssd1306_send_cmd(display, 0x22); // set page address
    ssd1306_send_cmd(display, 0);    // start page
    ssd1306_send_cmd(display, 7);    // end page
}

void display_init(display_t *display, i2c_master_bus_handle_t bus_handle, uint8_t addr)
{
    display->addr = addr;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };

    i2c_master_bus_add_device(bus_handle, &dev_cfg, &display->dev_handle);

    // SSD1306 initialization sequence
    vTaskDelay(pdMS_TO_TICKS(100)); // wait for display power-up

    ssd1306_send_cmd(display, 0xAE); // display off
    ssd1306_send_cmd(display, 0xD5); // set display clock divide
    ssd1306_send_cmd(display, 0x80); // default ratio
    ssd1306_send_cmd(display, 0xA8); // set multiplex
    ssd1306_send_cmd(display, 0x3F); // 64 rows (for 128x64)
    ssd1306_send_cmd(display, 0xD3); // set display offset
    ssd1306_send_cmd(display, 0x00); // no offset
    ssd1306_send_cmd(display, 0x40); // set start line to 0
    ssd1306_send_cmd(display, 0x8D); // charge pump
    ssd1306_send_cmd(display, 0x14); // enable charge pump
    ssd1306_send_cmd(display, 0x20); // memory addressing mode
    ssd1306_send_cmd(display, 0x00); // horizontal addressing
    ssd1306_send_cmd(display, 0xA1); // segment remap (flip horizontal)
    ssd1306_send_cmd(display, 0xC8); // COM scan direction (flip vertical)
    ssd1306_send_cmd(display, 0xDA); // set COM pins
    ssd1306_send_cmd(display, 0x12); // alternative COM pin config (128x64)
    ssd1306_send_cmd(display, 0x81); // set contrast
    ssd1306_send_cmd(display, 0xCF); // max contrast
    ssd1306_send_cmd(display, 0xD9); // set pre-charge period
    ssd1306_send_cmd(display, 0xF1);
    ssd1306_send_cmd(display, 0xDB); // set VCOMH deselect level
    ssd1306_send_cmd(display, 0x40);
    ssd1306_send_cmd(display, 0xA4); // display from RAM
    ssd1306_send_cmd(display, 0xA6); // normal display (not inverted)
    ssd1306_send_cmd(display, 0xAF); // display on
}

void display_test(display_t *display)
{
    uint8_t page[128];

    // Fill screen white
    ssd1306_reset_cursor(display);
    memset(page, 0xFF, sizeof(page));
    for (int p = 0; p < 8; p++) {
        ssd1306_send_data(display, page, 128);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    // Clear screen
    ssd1306_reset_cursor(display);
    memset(page, 0x00, sizeof(page));
    for (int p = 0; p < 8; p++) {
        ssd1306_send_data(display, page, 128);
    }
}