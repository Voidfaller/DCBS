# Display Driver Test Report

## Overview

The **Display Driver** module controls an SSD1306 OLED display over I2C.  
It is responsible for display initialization, screen buffer management, drawing primitives, UI input handling, UI rendering, and starting the display task.

This report documents the executed tests and their results.

The module under test exposes the following public interface:

- `esp_err_t display_init(display_t *display, uint8_t addr, int width, int height)`
- `void display_test(display_t *display)`
- `void display_ui_init(display_t *display, const display_ui_setting_t *settings, size_t setting_count)`
- `void display_clear(display_t *display)`
- `void display_flush(display_t *display)`
- `void display_drawPixel(display_t *display, int x, int y, uint8_t color)`
- `void display_drawLine(display_t *display, int x0, int y0, int x1, int y1, uint8_t color)`
- `void display_drawRect(display_t *display, int x, int y, int w, int h, uint8_t color)`
- `void display_fillRect(display_t *display, int x, int y, int w, int h, uint8_t color)`
- `void display_ui_handle_input(display_t *display, bool up_pressed, bool down_pressed, bool select_pressed, bool back_pressed)`
- `void display_ui_render(display_t *display, const display_ui_data_t *ui_data)`
- `esp_err_t display_ui_start_task(uint8_t display_addr, int width, int height, const display_ui_setting_t *settings, size_t setting_count)`
- `esp_err_t display_driver_start(void)`

---

## Test Environment

| Item | Description |
|------|-------------|
| Target platform | ESP32 |
| Framework | ESP-IDF / FreeRTOS |
| Module under test | `display_driver.c` / `display_driver.h` |
| Related modules | `I2C_wrapper`, `gyro_driver`, `pid_control`, `servo_driver`, GPIO driver |
| Display type | SSD1306 OLED |
| Test method | Functional API testing and runtime validation |

---

## Test Results

### **Test1 – Display Initialization**

**Objective:** Verify that the display initializes correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_init(&display, 0x3C, 128, 64)` |
| Expected Output | Function returns `ESP_OK`; display buffer and UI state are initialized |
| Actual Output | Display initialized successfully |
| Result | Pass |

**Steps:**
1. Power on the board.
2. Call `display_init(&display, 0x3C, 128, 64)`.
3. Check the return value.
4. Verify that the display responds and shows a cleared frame.

---

### **Test2 – Invalid Display Pointer Handling**

**Objective:** Verify safe handling of an invalid display pointer.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_init(NULL, 0x3C, 128, 64)` |
| Expected Output | Function returns `ESP_ERR_INVALID_ARG` |
| Actual Output | Invalid argument was handled correctly |
| Result | Pass |

**Steps:**
1. Call `display_init(NULL, 0x3C, 128, 64)`.
2. Check the return value.
3. Verify that no crash occurs.

---

### **Test3 – Clear Display Buffer**

**Objective:** Verify that the display shadow buffer is cleared correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_clear(&display)` |
| Expected Output | All bytes in `shadow_ram` are set to `0` |
| Actual Output | Display buffer was cleared correctly |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Draw visible content into the shadow buffer.
3. Call `display_clear(&display)`.
4. Verify that the shadow buffer is empty.

---

### **Test4 – Flush Display Buffer**

**Objective:** Verify that the display shadow buffer is transmitted to the OLED.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_flush(&display)` |
| Expected Output | Current buffer content becomes visible on the display |
| Actual Output | Buffer was flushed successfully |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Draw test content into the shadow buffer.
3. Call `display_flush(&display)`.
4. Verify that the expected image is shown on the screen.

---

### **Test5 – Draw Pixel**

**Objective:** Verify that a single pixel can be drawn correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_drawPixel(&display, 0, 0, 1)` |
| Expected Output | One pixel is set in the display buffer |
| Actual Output | Pixel was drawn correctly |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Call `display_drawPixel(&display, 0, 0, 1)`.
3. Flush the display.
4. Verify that the pixel is visible.

---

### **Test6 – Draw Line**

**Objective:** Verify that a line is drawn correctly between two coordinates.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_drawLine(&display, -10, 0, 10, 0, 1)` |
| Expected Output | A horizontal line appears on the display |
| Actual Output | Line was drawn correctly |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Call `display_drawLine(&display, -10, 0, 10, 0, 1)`.
3. Flush the display.
4. Verify that the expected line is shown.

---

### **Test7 – Draw Rectangle**

**Objective:** Verify that a rectangle outline is drawn correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_drawRect(&display, 0, 0, 20, 10, 1)` |
| Expected Output | Rectangle outline appears on the display |
| Actual Output | Rectangle was drawn correctly |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Call `display_drawRect(&display, 0, 0, 20, 10, 1)`.
3. Flush the display.
4. Verify that the expected rectangle is visible.

---

### **Test8 – Fill Rectangle**

**Objective:** Verify that a filled rectangle is drawn correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_fillRect(&display, 0, 0, 20, 10, 1)` |
| Expected Output | Filled rectangle appears on the display |
| Actual Output | Filled rectangle was drawn correctly |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Call `display_fillRect(&display, 0, 0, 20, 10, 1)`.
3. Flush the display.
4. Verify that the rectangle area is filled.

---

### **Test9 – UI Initialization**

**Objective:** Verify that the UI state is initialized correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_ui_init(&display, settings, setting_count)` |
| Expected Output | UI settings, selection state, and default screen are initialized |
| Actual Output | UI state initialized correctly |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Define a valid settings array.
3. Call `display_ui_init(&display, settings, setting_count)`.
4. Verify that the default screen and menu state are set correctly.

---

### **Test10 – UI Input Navigation**

**Objective:** Verify that button input changes screens and menu state correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_ui_handle_input()` with simulated button events |
| Expected Output | UI screen or selection changes according to input |
| Actual Output | UI navigation behaved correctly |
| Result | Pass |

**Steps:**
1. Initialize the display UI.
2. Simulate `up`, `down`, `select`, and `back` button presses.
3. Verify screen switching and menu behavior.
4. Verify that setting selection changes as expected.

---

### **Test11 – UI Rendering**

**Objective:** Verify that UI data is rendered correctly to the display buffer.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_ui_render(&display, &ui_data)` |
| Expected Output | Corresponding UI screen is drawn into the shadow buffer |
| Actual Output | UI screen rendered correctly |
| Result | Pass |

**Steps:**
1. Initialize the display and UI.
2. Create a valid `display_ui_data_t` instance.
3. Call `display_ui_render(&display, &ui_data)`.
4. Flush the display and verify that the correct screen is shown.

---

### **Test12 – UI Render With Null Data**

**Objective:** Verify safe handling of missing UI data.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_ui_render(&display, NULL)` |
| Expected Output | Display shows fallback message or remains stable |
| Actual Output | Null input was handled safely |
| Result | Pass |

**Steps:**
1. Initialize the display.
2. Call `display_ui_render(&display, NULL)`.
3. Verify that the system remains stable and no crash occurs.

---

### **Test13 – Start Display UI Task**

**Objective:** Verify that the display UI task starts correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_ui_start_task(0x3C, 128, 64, settings, setting_count)` |
| Expected Output | Function returns `ESP_OK` and UI task starts |
| Actual Output | Display UI task started successfully |
| Result | Pass |

**Steps:**
1. Prepare the settings array.
2. Call `display_ui_start_task(0x3C, 128, 64, settings, setting_count)`.
3. Check the return value.
4. Verify that the display updates periodically.

---

### **Test14 – Prevent Double Start of UI Task**

**Objective:** Verify that the UI task cannot be started twice.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_ui_start_task(...)` twice |
| Expected Output | Second call returns `ESP_ERR_INVALID_STATE` |
| Actual Output | Duplicate start was blocked correctly |
| Result | Pass |

**Steps:**
1. Start the display UI task once.
2. Call `display_ui_start_task(...)` again.
3. Verify that the second call returns `ESP_ERR_INVALID_STATE`.

---

### **Test15 – Display Driver Start**

**Objective:** Verify that the display driver starts using the configured I2C addresses.

| Parameter | Value |
|-----------|-------|
| Input | Call `display_driver_start()` |
| Expected Output | Driver starts successfully on `0x3C` or falls back to `0x3D` |
| Actual Output | Driver started successfully |
| Result | Pass |

**Steps:**
1. Power on the board.
2. Call `display_driver_start()`.
3. Check the return value.
4. Verify that the display becomes active.

---

## Conclusion

Based on the executed tests, the Display Driver module functions correctly.  
Initialization, drawing primitives, buffer handling, UI state handling, UI rendering, and task startup all behaved as expected.

The module is therefore suitable for integration into the embedded control system.

---