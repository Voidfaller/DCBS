#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

typedef enum
{
    DISPLAY_UI_SCREEN_PID = 0,
    DISPLAY_UI_SCREEN_GYRO,
    DISPLAY_UI_SCREEN_ESTIMATOR,
    DISPLAY_UI_SCREEN_SERVO,
    DISPLAY_UI_SCREEN_STATS,
    DISPLAY_UI_SCREEN_MENU,
} display_ui_screen_t;

typedef struct
{
    int32_t pid_setpoint;
    int32_t pid_measured;
    int32_t pid_output;
    int32_t gyro_x_dps;
    int32_t gyro_y_dps;
    int32_t gyro_z_dps;
    int32_t est_pitch_deg;
    int32_t est_roll_deg;
    int32_t servo_pwm_us;
    uint32_t uptime_s;
    uint32_t free_heap_bytes;
    uint32_t frame_hz;
} display_ui_data_t;

typedef struct
{
    const char *name;
    int32_t *value;
    int32_t min;
    int32_t max;
    int32_t step;
} display_ui_setting_t;

#define DISPLAY_UI_SETTING(NAME, VALUE_PTR, MIN, MAX, STEP) \
    { (NAME), (VALUE_PTR), (MIN), (MAX), (STEP) }

typedef struct
{
    int SCL_GPIO;
    int SDA_GPIO;
    uint8_t addr;
    i2c_master_dev_handle_t dev_handle;
    int width;
    int height;
    uint8_t shadow_ram[1024];  // Shadow buffer: 8 pages x 128 columns for 128x64 displays
    const display_ui_setting_t *settings;
    size_t setting_count;
    size_t selected_setting;
    bool menu_edit_mode;
    display_ui_screen_t ui_screen;
    display_ui_screen_t return_screen;
    int16_t pid_history[64];
    int16_t pid_output_history[64];
    uint8_t pid_history_head;
} display_t;

esp_err_t display_init(display_t *display, uint8_t addr, int width, int height);
void display_test(display_t *display);
void display_clear(display_t *display);
void display_flush(display_t *display);
void display_drawPixel(display_t *display, int x, int y, uint8_t color);
void display_drawLine(display_t *display, int x0, int y0, int x1, int y1, uint8_t color);
void display_drawRect(display_t *display, int x, int y, int w, int h, uint8_t color);
void display_fillRect(display_t *display, int x, int y, int w, int h, uint8_t color);
void display_ui_init(display_t *display, const display_ui_setting_t *settings, size_t setting_count);
void display_ui_handle_input(display_t *display, bool up_pressed, bool down_pressed, bool select_pressed, bool back_pressed);
void display_ui_render(display_t *display, const display_ui_data_t *ui_data);
esp_err_t display_ui_start_task(uint8_t display_addr,
                                int width,
                                int height,
                                const display_ui_setting_t *settings,
                                size_t setting_count);

esp_err_t display_driver_start(void);