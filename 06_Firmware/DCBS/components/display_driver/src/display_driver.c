#include "display_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

#define I2C_XFER_TIMEOUT_MS 100
#define UI_HISTORY_LEN 64
#define UI_TASK_STACK_SIZE 4096
#define UI_TASK_PRIORITY 5
#define UI_TASK_FRAME_MS 50
#define UI_BTN_UP_GPIO 25
#define UI_BTN_DOWN_GPIO 26
#define UI_BTN_SELECT_GPIO 27
#define UI_BTN_BACK_GPIO 14

typedef struct
{
    bool up;
    bool down;
    bool select;
    bool back;
} ui_button_events_t;

typedef struct
{
    display_t display;
    const display_ui_setting_t *settings;
    size_t setting_count;
    int frame;
    bool running;
} display_ui_task_ctx_t;

static display_ui_task_ctx_t s_ui_task_ctx;

static int ui_triangle_wave(int frame, int period, int amplitude)
{
    int phase = frame % (2 * period);
    int ramp = (phase < period) ? phase : (2 * period - phase);
    return ((ramp * 2 * amplitude) / period) - amplitude;
}

static void ui_buttons_init(void)
{
    gpio_config_t buttons_cfg = {
        .pin_bit_mask = (1ULL << UI_BTN_UP_GPIO) |
                        (1ULL << UI_BTN_DOWN_GPIO) |
                        (1ULL << UI_BTN_SELECT_GPIO) |
                        (1ULL << UI_BTN_BACK_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&buttons_cfg));
}

static ui_button_events_t ui_read_button_events(void)
{
    static bool prev_up = false;
    static bool prev_down = false;
    static bool prev_select = false;
    static bool prev_back = false;
    ui_button_events_t events = {0};

    bool up_now = (gpio_get_level(UI_BTN_UP_GPIO) == 1);
    bool down_now = (gpio_get_level(UI_BTN_DOWN_GPIO) == 1);
    bool select_now = (gpio_get_level(UI_BTN_SELECT_GPIO) == 1);
    bool back_now = (gpio_get_level(UI_BTN_BACK_GPIO) == 1);

    events.up = up_now && !prev_up;
    events.down = down_now && !prev_down;
    events.select = select_now && !prev_select;
    events.back = back_now && !prev_back;

    prev_up = up_now;
    prev_down = down_now;
    prev_select = select_now;
    prev_back = back_now;

    return events;
}

static int32_t clamp_i32(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

static void ssd1306_send_cmd(display_t *display, uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd}; // Co=0, D/C#=0 (command)
    esp_err_t ret = i2c_master_transmit(display->dev_handle, buf, 2, I2C_XFER_TIMEOUT_MS);
    if (ret != ESP_OK)
    {
        printf("ssd1306_send_cmd failed: 0x%x (cmd=0x%02X)\n", ret, cmd);
    }
}

static void ssd1306_send_data(display_t *display, const uint8_t *data, size_t len)
{
    // Send in chunks of 32 bytes to avoid I2C timeout
    size_t offset = 0;
    while (offset < len)
    {
        size_t chunk = (len - offset > 32) ? 32 : (len - offset);
        uint8_t buf[33];
        buf[0] = 0x40; // Co=0, D/C#=1 (data)
        memcpy(&buf[1], data + offset, chunk);
        esp_err_t ret = i2c_master_transmit(display->dev_handle, buf, chunk + 1, I2C_XFER_TIMEOUT_MS);
        if (ret != ESP_OK)
        {
            printf("ssd1306_send_data failed: 0x%x\n", ret);
            break;
        }
        offset += chunk;
    }
}

static void display_drawPixelAbs(display_t *display, int display_x, int display_y, uint8_t color)
{
    if (display_x < 0 || display_x >= display->width || display_y < 0 || display_y >= display->height)
    {
        return;
    }

    uint8_t page = (uint8_t)(display_y / 8);
    uint8_t bit = (uint8_t)(1U << (display_y % 8));
    size_t shadow_index = ((size_t)page * (size_t)display->width) + (size_t)display_x;
    uint8_t pixel_byte = display->shadow_ram[shadow_index];

    if (color)
    {
        pixel_byte |= bit;
    }
    else
    {
        pixel_byte &= (uint8_t)(~bit);
    }

    display->shadow_ram[shadow_index] = pixel_byte;
}

static void ui_draw_char(display_t *display, int x, int y, char c)
{
    static const uint8_t font_digits[10][5] = {
        {0x3E, 0x51, 0x49, 0x45, 0x3E}, {0x00, 0x42, 0x7F, 0x40, 0x00}, {0x62, 0x51, 0x49, 0x49, 0x46}, {0x22, 0x49, 0x49, 0x49, 0x36}, {0x18, 0x14, 0x12, 0x7F, 0x10},
        {0x2F, 0x49, 0x49, 0x49, 0x31}, {0x3E, 0x49, 0x49, 0x49, 0x32}, {0x01, 0x71, 0x09, 0x05, 0x03}, {0x36, 0x49, 0x49, 0x49, 0x36}, {0x26, 0x49, 0x49, 0x49, 0x3E}};
    static const uint8_t font_upper[26][5] = {
        {0x7E, 0x11, 0x11, 0x11, 0x7E}, {0x7F, 0x49, 0x49, 0x49, 0x36}, {0x3E, 0x41, 0x41, 0x41, 0x22}, {0x7F, 0x41, 0x41, 0x22, 0x1C}, {0x7F, 0x49, 0x49, 0x49, 0x41},
        {0x7F, 0x09, 0x09, 0x09, 0x01}, {0x3E, 0x41, 0x49, 0x49, 0x7A}, {0x7F, 0x08, 0x08, 0x08, 0x7F}, {0x00, 0x41, 0x7F, 0x41, 0x00}, {0x20, 0x40, 0x41, 0x3F, 0x01},
        {0x7F, 0x08, 0x14, 0x22, 0x41}, {0x7F, 0x40, 0x40, 0x40, 0x40}, {0x7F, 0x02, 0x0C, 0x02, 0x7F}, {0x7F, 0x04, 0x08, 0x10, 0x7F}, {0x3E, 0x41, 0x41, 0x41, 0x3E},
        {0x7F, 0x09, 0x09, 0x09, 0x06}, {0x3E, 0x41, 0x51, 0x21, 0x5E}, {0x7F, 0x09, 0x19, 0x29, 0x46}, {0x46, 0x49, 0x49, 0x49, 0x31}, {0x01, 0x01, 0x7F, 0x01, 0x01},
        {0x3F, 0x40, 0x40, 0x40, 0x3F}, {0x1F, 0x20, 0x40, 0x20, 0x1F}, {0x3F, 0x40, 0x38, 0x40, 0x3F}, {0x63, 0x14, 0x08, 0x14, 0x63}, {0x03, 0x04, 0x78, 0x04, 0x03}, {0x61, 0x51, 0x49, 0x45, 0x43}};
    static const uint8_t glyph_space[5] = {0, 0, 0, 0, 0};
    static const uint8_t glyph_dash[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const uint8_t glyph_dot[5] = {0x00, 0x60, 0x60, 0x00, 0x00};
    static const uint8_t glyph_colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const uint8_t glyph_gt[5] = {0x00, 0x41, 0x22, 0x14, 0x08};

    const uint8_t *glyph = glyph_space;
    char uc = (char)toupper((unsigned char)c);

    if (uc >= '0' && uc <= '9')
    {
        glyph = font_digits[(int)(uc - '0')];
    }
    else if (uc >= 'A' && uc <= 'Z')
    {
        glyph = font_upper[(int)(uc - 'A')];
    }
    else if (uc == '-')
    {
        glyph = glyph_dash;
    }
    else if (uc == '.')
    {
        glyph = glyph_dot;
    }
    else if (uc == ':')
    {
        glyph = glyph_colon;
    }
    else if (uc == '>')
    {
        glyph = glyph_gt;
    }

    for (int col = 0; col < 5; col++)
    {
        uint8_t bits = glyph[col];
        for (int row = 0; row < 7; row++)
        {
            if ((bits >> row) & 0x01U)
            {
                display_drawPixelAbs(display, x + col, y + row, 1);
            }
        }
    }
}

static void ui_draw_text(display_t *display, int center_x, int center_y, const char *text)
{
    if (text == NULL)
    {
        return;
    }

    int len = 0;
    while (text[len] != '\0')
    {
        len++;
    }

    int abs_x = center_x + (display->width / 2) - ((len * 6) / 2);
    int abs_y = center_y + (display->height / 2) - 3;
    for (int i = 0; i < len; i++)
    {
        ui_draw_char(display, abs_x + i * 6, abs_y, text[i]);
    }
}

static void ui_draw_int(display_t *display, int center_x, int center_y, int32_t value)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%ld", (long)value);
    ui_draw_text(display, center_x, center_y, buf);
}

static void ui_draw_bar(display_t *display, int center_x, int center_y, int width, int height, int32_t value, int32_t min, int32_t max)
{
    int32_t clamped = clamp_i32(value, min, max);
    int32_t range = max - min;
    int fill = 0;

    display_drawRect(display, center_x, center_y, width, height, 1);
    if (range <= 0)
    {
        return;
    }

    fill = (int)(((clamped - min) * (width - 2)) / range);
    if (fill > 0)
    {
        int left = center_x - (width / 2) + 1;
        display_fillRect(display, left + (fill / 2), center_y, fill, height - 2, 1);
    }
}

static void ui_draw_screen_header(display_t *display, const char *title, int screen_index)
{
    char buf[20];
    display_drawRect(display, 0, 0, 126, 62, 1);
    display_drawLine(display, -63, -22, 63, -22, 1);
    ui_draw_text(display, -44, -27, title);
    snprintf(buf, sizeof(buf), "SCR %d OF 4", screen_index);
    ui_draw_text(display, 24, -27, buf);
}

static void ui_draw_pid_screen(display_t *display, int32_t pid_setpoint, int32_t pid_measured, int32_t pid_output)
{
    int graph_x0 = -58;
    int graph_x1 = 34;
    int graph_y0 = -18;
    int graph_y1 = 10;
    int graph_w = graph_x1 - graph_x0;
    int graph_mid = (graph_y0 + graph_y1) / 2;
    int32_t setpoint_clamped = clamp_i32(pid_setpoint, -100, 100);
    int32_t measured_clamped = clamp_i32(pid_measured, -100, 100);
    int32_t output_clamped = clamp_i32(pid_output, -100, 100);

    ui_draw_screen_header(display, "PID", 1);

    display_drawRect(display, (graph_x0 + graph_x1) / 2, (graph_y0 + graph_y1) / 2, graph_w, graph_y1 - graph_y0, 1);

    display->pid_history[display->pid_history_head] = (int16_t)output_clamped;
    display->pid_history_head = (uint8_t)((display->pid_history_head + 1U) % UI_HISTORY_LEN);

    int setpoint_y = graph_mid - (int)((setpoint_clamped * 10) / 100);
    int measured_y = graph_mid - (int)((measured_clamped * 10) / 100);
    display_drawLine(display, graph_x0 + 1, setpoint_y, graph_x1 - 1, setpoint_y, 1);
    display_drawLine(display, graph_x1 + 6, measured_y - 1, graph_x1 + 12, measured_y - 1, 1);
    display_drawLine(display, graph_x1 + 6, measured_y, graph_x1 + 12, measured_y, 1);
    display_drawLine(display, graph_x1 + 6, measured_y + 1, graph_x1 + 12, measured_y + 1, 1);

    for (int i = 1; i < 46; i++)
    {
        int idx0 = (int)((display->pid_history_head + UI_HISTORY_LEN - 46 + i - 1) % UI_HISTORY_LEN);
        int idx1 = (int)((display->pid_history_head + UI_HISTORY_LEN - 46 + i) % UI_HISTORY_LEN);
        int y0 = graph_mid - (display->pid_history[idx0] * 10) / 100;
        int y1 = graph_mid - (display->pid_history[idx1] * 10) / 100;
        int x0 = graph_x0 + 1 + (i - 1) * 2;
        int x1 = graph_x0 + 1 + i * 2;
        display_drawLine(display, x0, y0, x1, y1, 1);
    }

    display_drawRect(display, 0, 22, 120, 14, 1);
    ui_draw_text(display, -50, 22, "OUT");
    ui_draw_int(display, -23, 22, output_clamped);

    int bar_half = (int)((output_clamped * 45) / 100);
    if (bar_half >= 0)
    {
        display_fillRect(display, -45 + bar_half, 22, bar_half * 2, 7, 1);
    }
    else
    {
        int width = -bar_half * 2;
        display_fillRect(display, -45 - (width / 2), 22, width, 7, 1);
    }
}

static void ui_draw_gyro_screen(display_t *display, int32_t gyro_x, int32_t gyro_y, int32_t gyro_z)
{
    int cx = 0;
    int cy = 8;
    int length = 16;
    int32_t z_clamped = clamp_i32(gyro_z, -500, 500);
    float angle_deg = ((float)z_clamped / 500.0f) * 45.0f;
    float radians = angle_deg * 0.0174532925f;
    int dx = (int)(cosf(radians) * (float)length);
    int dy = (int)(sinf(radians) * (float)length);

    ui_draw_screen_header(display, "GYRO", 2);

    ui_draw_text(display, -48, -12, "X");
    ui_draw_int(display, -34, -12, gyro_x);
    ui_draw_text(display, -2, -12, "Y");
    ui_draw_int(display, 12, -12, gyro_y);
    ui_draw_text(display, 42, -12, "Z");
    ui_draw_int(display, 54, -12, gyro_z);

    display_drawRect(display, 0, 8, 86, 24, 1);
    display_drawLine(display, cx - 30, cy, cx + 30, cy, 1);
    display_drawLine(display, cx, cy - 10, cx, cy + 10, 1);
    display_drawLine(display, cx - dx, cy + dy, cx + dx, cy - dy, 1);

    ui_draw_text(display, -56, 22, "X");
    ui_draw_bar(display, -32, 22, 30, 8, gyro_x, -500, 500);
    ui_draw_text(display, -4, 22, "Y");
    ui_draw_bar(display, 20, 22, 30, 8, gyro_y, -500, 500);
    ui_draw_text(display, 48, 22, "Z");
    ui_draw_bar(display, 56, 22, 14, 8, gyro_z, -500, 500);
}

static void ui_draw_servo_screen(display_t *display, int32_t pwm_us)
{
    int32_t pwm = clamp_i32(pwm_us, 500, 2500);
    int x0 = -58;
    int x1 = 58;
    int y_high = -4;
    int y_low = 12;
    int prev_x = x0;
    int prev_y = y_low;

    ui_draw_screen_header(display, "SERVO", 3);
    ui_draw_text(display, -42, -12, "PWM US");
    ui_draw_int(display, 10, -12, pwm);

    display_drawRect(display, 0, 6, 118, 24, 1);
    for (int x = x0; x <= x1; x++)
    {
        int t_us = ((x - x0) * 40000) / (x1 - x0 + 1);
        int phase_us = t_us % 20000;
        int y = (phase_us < pwm) ? y_high : y_low;

        if (x > x0)
        {
            display_drawLine(display, prev_x, prev_y, x, y, 1);
        }
        prev_x = x;
        prev_y = y;
    }

    ui_draw_bar(display, 0, 22, 96, 8, pwm, 500, 2500);
}

static void ui_draw_stats_screen(display_t *display, uint32_t uptime_s, uint32_t free_heap_bytes, uint32_t frame_hz)
{
    ui_draw_screen_header(display, "STATS", 4);

    ui_draw_text(display, -46, -10, "UPTIME S");
    ui_draw_int(display, 20, -10, (int32_t)uptime_s);
    ui_draw_text(display, -46, 0, "HEAP KB");
    ui_draw_int(display, 20, 0, (int32_t)(free_heap_bytes / 1024U));
    ui_draw_text(display, -46, 10, "UI FPS");
    ui_draw_int(display, 20, 10, (int32_t)frame_hz);

    display_drawRect(display, 0, 23, 120, 10, 1);
    ui_draw_text(display, 0, 23, (free_heap_bytes < (80U * 1024U)) ? "LOW MEMORY" : "SYSTEM OK");
}

static void ui_draw_menu_screen(display_t *display)
{
    display_drawRect(display, 0, 0, 126, 62, 1);
    display_drawLine(display, -63, -22, 63, -22, 1);
    ui_draw_text(display, -30, -27, "SETTINGS");
    ui_draw_text(display, 42, -27, display->menu_edit_mode ? "EDIT" : "NAV");

    if (display->setting_count == 0)
    {
        ui_draw_text(display, 0, 0, "NO SETTINGS");
        return;
    }

    int start = (int)display->selected_setting - 1;
    if (start < 0)
    {
        start = 0;
    }
    if (start > (int)display->setting_count - 3)
    {
        start = (int)display->setting_count - 3;
        if (start < 0)
        {
            start = 0;
        }
    }

    for (int row = 0; row < 3; row++)
    {
        int idx = start + row;
        if (idx >= (int)display->setting_count)
        {
            break;
        }

        int y = -10 + row * 12;
        if ((size_t)idx == display->selected_setting)
        {
            ui_draw_text(display, -58, y, ">");
        }

        ui_draw_text(display, -28, y, display->settings[idx].name);
        ui_draw_int(display, 42, y, *display->settings[idx].value);
    }
}

void display_clear(display_t *display)
{
    size_t shadow_size = (size_t)display->width * ((size_t)display->height / 8U);
    memset(display->shadow_ram, 0x00, shadow_size);
}

void display_flush(display_t *display)
{
    int pages = display->height / 8;

    for (int page = 0; page < pages; page++)
    {
        size_t page_offset = (size_t)page * (size_t)display->width;
        ssd1306_send_cmd(display, (uint8_t)(0xB0 + page));
        ssd1306_send_cmd(display, 0x00);
        ssd1306_send_cmd(display, 0x10);
        ssd1306_send_data(display, &display->shadow_ram[page_offset], (size_t)display->width);
    }
}

void display_init(display_t *display, i2c_master_bus_handle_t bus_handle, uint8_t addr, int width, int height)
{
    display->addr = addr;
    display->width = width;
    display->height = height;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &display->dev_handle));

    // SSD1306 initialization sequence
    vTaskDelay(pdMS_TO_TICKS(100));  // wait for display power-up
    ssd1306_send_cmd(display, 0xAE); // display off
    ssd1306_send_cmd(display, 0xA3); // set MUX ratio
    ssd1306_send_cmd(display, 0x00); // 0 static area
    ssd1306_send_cmd(display, 0x40); // 64 scroll area
    ssd1306_send_cmd(display, 0xD3); // set display offset
    ssd1306_send_cmd(display, 0x00); // no offset
    ssd1306_send_cmd(display, 0x40); // set display start line
    ssd1306_send_cmd(display, 0xA0); // set segment remap
    ssd1306_send_cmd(display, 0xC0); // set COM output scan direction
    ssd1306_send_cmd(display, 0xDA); // set COM pins hardware configuration
    ssd1306_send_cmd(display, 0x12); // alternative COM pin config
    ssd1306_send_cmd(display, 0x81); // set contrast control
    ssd1306_send_cmd(display, 0xA7); // contrast value
    // ssd1306_send_cmd(display, 0xA5); // enable entire display on
    ssd1306_send_cmd(display, 0xA4); // disable entire display on (resume to RAM content)
    ssd1306_send_cmd(display, 0xA6); // set normal display (not inverted)
    ssd1306_send_cmd(display, 0xD5); // set display clock divide ratio/oscillator frequency
    ssd1306_send_cmd(display, 0x80); // default clock divide ratio
    ssd1306_send_cmd(display, 0x8D); // set charge pump
    ssd1306_send_cmd(display, 0x14); // enable charge pump

    ssd1306_send_cmd(display, 0xAF); // display on

    // Initialize shadow RAM
    memset(display->shadow_ram, 0x00, sizeof(display->shadow_ram));
    memset(display->pid_history, 0x00, sizeof(display->pid_history));
    display->pid_history_head = 0;
    display->settings = NULL;
    display->setting_count = 0;
    display->selected_setting = 0;
    display->menu_edit_mode = false;
    display->ui_screen = DISPLAY_UI_SCREEN_PID;
    display->return_screen = DISPLAY_UI_SCREEN_PID;

    display_flush(display);
}

void display_drawPixel(display_t *display, int x, int y, uint8_t color) // color: 0=off, 1=on
{
    // Transform user coordinates to display coordinates
    // User (0, 0) = center of display (64, 32)
    // Positive X = right, Positive Y = down
    int display_x = x + (display->width / 2);
    int display_y = y + (display->height / 2);

    // Bounds check
    if (display_x < 0 || display_x >= display->width || display_y < 0 || display_y >= display->height)
    {
        return;
    }

    display_drawPixelAbs(display, display_x, display_y, color);
}

void display_drawLine(display_t *display, int x0, int y0, int x1, int y1, uint8_t color)
{
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        display_drawPixel(display, x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void display_drawRect(display_t *display, int x, int y, int width, int height, uint8_t color)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    int left = x - ((width - 1) / 2);
    int right = left + width - 1;
    int top = y - ((height - 1) / 2);
    int bottom = top + height - 1;

    display_drawLine(display, left, top, right, top, color);       // Top
    display_drawLine(display, left, bottom, right, bottom, color); // Bottom
    display_drawLine(display, left, top, left, bottom, color);     // Left
    display_drawLine(display, right, top, right, bottom, color);   // Right
}

void display_fillRect(display_t *display, int x, int y, int width, int height, uint8_t color)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    int left = x - ((width - 1) / 2);
    int right = left + width - 1;
    int top = y - ((height - 1) / 2);

    for (int i = 0; i < height; i++)
    {
        int row = top + i;
        display_drawLine(display, left, row, right, row, color);
    }
}

void display_ui_init(display_t *display, const display_ui_setting_t *settings, size_t setting_count)
{
    display->settings = settings;
    display->setting_count = setting_count;
    display->selected_setting = 0;
    display->menu_edit_mode = false;
    display->ui_screen = DISPLAY_UI_SCREEN_PID;
    display->return_screen = DISPLAY_UI_SCREEN_PID;
}

void display_ui_handle_input(display_t *display, bool up_pressed, bool down_pressed, bool select_pressed, bool back_pressed)
{
    if (display->ui_screen != DISPLAY_UI_SCREEN_MENU)
    {
        if (up_pressed)
        {
            if (display->ui_screen == DISPLAY_UI_SCREEN_PID)
            {
                display->ui_screen = DISPLAY_UI_SCREEN_STATS;
            }
            else
            {
                display->ui_screen = (display_ui_screen_t)((int)display->ui_screen - 1);
            }
        }
        if (down_pressed)
        {
            if (display->ui_screen == DISPLAY_UI_SCREEN_STATS)
            {
                display->ui_screen = DISPLAY_UI_SCREEN_PID;
            }
            else
            {
                display->ui_screen = (display_ui_screen_t)((int)display->ui_screen + 1);
            }
        }
    }

    if (select_pressed)
    {
        if (display->ui_screen != DISPLAY_UI_SCREEN_MENU)
        {
            display->return_screen = display->ui_screen;
            display->ui_screen = DISPLAY_UI_SCREEN_MENU;
            display->menu_edit_mode = false;
        }
        else if (display->setting_count > 0)
        {
            display->menu_edit_mode = !display->menu_edit_mode;
        }
    }

    if (back_pressed)
    {
        if (display->ui_screen == DISPLAY_UI_SCREEN_MENU)
        {
            if (display->menu_edit_mode)
            {
                display->menu_edit_mode = false;
            }
            else
            {
                display->ui_screen = display->return_screen;
            }
        }
    }

    if (display->ui_screen != DISPLAY_UI_SCREEN_MENU || display->setting_count == 0)
    {
        return;
    }

    if (display->menu_edit_mode)
    {
        display_ui_setting_t setting = display->settings[display->selected_setting];
        int32_t delta = 0;
        if (up_pressed)
        {
            delta += setting.step;
        }
        if (down_pressed)
        {
            delta -= setting.step;
        }
        if (delta != 0)
        {
            int32_t next = *setting.value + delta;
            *setting.value = clamp_i32(next, setting.min, setting.max);
        }
    }
    else
    {
        if (up_pressed)
        {
            if (display->selected_setting == 0)
            {
                display->selected_setting = display->setting_count - 1;
            }
            else
            {
                display->selected_setting--;
            }
        }
        if (down_pressed)
        {
            display->selected_setting = (display->selected_setting + 1) % display->setting_count;
        }
    }
}

void display_ui_render(display_t *display, const display_ui_data_t *ui_data)
{
    display_clear(display);

    if (ui_data == NULL)
    {
        ui_draw_text(display, 0, 0, "NO UI DATA");
        return;
    }

    if (display->ui_screen == DISPLAY_UI_SCREEN_MENU)
    {
        ui_draw_menu_screen(display);
    }
    else if (display->ui_screen == DISPLAY_UI_SCREEN_GYRO)
    {
        ui_draw_gyro_screen(display, ui_data->gyro_x_dps, ui_data->gyro_y_dps, ui_data->gyro_z_dps);
    }
    else if (display->ui_screen == DISPLAY_UI_SCREEN_SERVO)
    {
        ui_draw_servo_screen(display, ui_data->servo_pwm_us);
    }
    else if (display->ui_screen == DISPLAY_UI_SCREEN_STATS)
    {
        ui_draw_stats_screen(display, ui_data->uptime_s, ui_data->free_heap_bytes, ui_data->frame_hz);
    }
    else
    {
        ui_draw_pid_screen(display, ui_data->pid_setpoint, ui_data->pid_measured, ui_data->pid_output);
    }
}

static void display_ui_task_run(void *arg)
{
    display_ui_task_ctx_t *ctx = (display_ui_task_ctx_t *)arg;

    ui_buttons_init();
    display_ui_init(&ctx->display, ctx->settings, ctx->setting_count);

    while (ctx->running)
    {
        ui_button_events_t events = ui_read_button_events();
        int32_t setpoint = (ctx->setting_count > 0 && ctx->settings[0].value != NULL) ? *ctx->settings[0].value : 0;
        int32_t kp = (ctx->setting_count > 1 && ctx->settings[1].value != NULL) ? *ctx->settings[1].value : 25;
        int32_t measured = ui_triangle_wave(ctx->frame, 120, 70);
        int32_t error = setpoint - measured;
        int32_t output = (kp * error) / 100;
        int32_t gyro_x = ui_triangle_wave(ctx->frame + 20, 80, 420);
        int32_t gyro_y = ui_triangle_wave(ctx->frame + 50, 110, 360);
        int32_t gyro_z = ui_triangle_wave(ctx->frame + 90, 140, 500);
        int32_t servo_pwm_us = 1500 + (output * 5);
        display_ui_data_t ui_data = {
            .pid_setpoint = setpoint,
            .pid_measured = measured,
            .pid_output = output,
            .gyro_x_dps = gyro_x,
            .gyro_y_dps = gyro_y,
            .gyro_z_dps = gyro_z,
            .servo_pwm_us = servo_pwm_us,
            .uptime_s = (uint32_t)(esp_timer_get_time() / 1000000ULL),
            .free_heap_bytes = heap_caps_get_free_size(MALLOC_CAP_8BIT),
            .frame_hz = (uint32_t)(1000 / UI_TASK_FRAME_MS),
        };

        display_ui_handle_input(&ctx->display, events.up, events.down, events.select, events.back);
        display_ui_render(&ctx->display, &ui_data);
        display_flush(&ctx->display);

        ctx->frame++;
        vTaskDelay(pdMS_TO_TICKS(UI_TASK_FRAME_MS));
    }

    vTaskDelete(NULL);
}

esp_err_t display_ui_start_task(i2c_master_bus_handle_t bus_handle,
                                uint8_t display_addr,
                                int width,
                                int height,
                                const display_ui_setting_t *settings,
                                size_t setting_count)
{
    BaseType_t created;

    if (s_ui_task_ctx.running)
    {
        return ESP_ERR_INVALID_STATE;
    }

    memset(&s_ui_task_ctx, 0, sizeof(s_ui_task_ctx));
    s_ui_task_ctx.settings = settings;
    s_ui_task_ctx.setting_count = setting_count;
    s_ui_task_ctx.frame = 0;
    s_ui_task_ctx.running = true;

    display_init(&s_ui_task_ctx.display, bus_handle, display_addr, width, height);

    created = xTaskCreate(display_ui_task_run,
                          "display_ui_task",
                          UI_TASK_STACK_SIZE,
                          &s_ui_task_ctx,
                          UI_TASK_PRIORITY,
                          NULL);

    if (created != pdPASS)
    {
        s_ui_task_ctx.running = false;
        return ESP_FAIL;
    }

    return ESP_OK;
}