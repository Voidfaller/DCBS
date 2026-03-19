# Gyro Driver Test Cases

## Overview

The **Gyro Driver** module reads data from the MPU6050 IMU sensor.  
It provides raw gyroscope (`gyro_x`, `gyro_y`, `gyro_z`) and accelerometer (`accel_x`, `accel_y`, `accel_z`) measurements for use in the State Estimator.

---

## Test Case Guidelines

- Use **ESP-IDF Unit Testing framework** or FreeRTOS test tasks.  
- Verify **sensor initialization**, **data reading**, and **error handling**.  
- Test under different conditions: normal operation, sensor disconnected, and I2C errors.

---

## Test Cases

### **Test1 – Initialization**

**Objective:** Ensure MPU6050 initializes correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `gyro_driver_init()` |
| Expected Output | Returns `ESP_OK` |
| Notes | If initialization fails, log error and return failure. |

**Steps:**
1. Power on the board.
2. Call `gyro_driver_init()`.
3. Check return value.
4. Verify MPU6050 registers are configured

---

### **test2 – Gyroscope Data Read**

**Objective:** Read gyro values in X, Y, Z axes.

| Parameter | Value |
|-----------|-------|
| Input | Call `gyro_driver_read_gyro(&gyro)` |
| Expected Output | Returns `ESP_OK`; values are floats within expected range (deg/s) |
| Notes | Gyro outputs should reflect actual rotation. |

**Steps:**
1. Initialize driver (`gyro_driver_init()`).
2. Call `gyro_driver_read_gyro(&gyro)`.
3. Verify all fields (`gyro_x`, `gyro_y`, `gyro_z`) are numeric.

---

### **test3 – Accelerometer Data Read**

**Objective:** Read accelerometer values in X, Y, Z axes.

| Parameter | Value |
|-----------|-------|
| Input | Call `gyro_driver_read_accel(&accel)` |
| Expected Output | Returns `ESP_OK`; values are floats in g units (-1 to 1g at rest) |
| Notes | Accelerometer should measure gravity when static. |

**Steps:**
1. Initialize driver (`gyro_driver_init()`).
2. Call `gyro_driver_read_accel(&accel)`.
3. Verify all fields (`accel_x`, `accel_y`, `accel_z`) are numeric.
4. Tilt board and confirm readings change accordingly.

---

### **test4 – Error Handling (I2C Failure)**

**Objective:** Confirm proper error handling when I2C read/write fails.

| Parameter | Value |
|-----------|-------|
| Input | Simulate I2C failure (sensor disconnected) |
| Expected Output | Functions return error (`ESP_ERR_INVALID_RESPONSE` or similar) |
| Notes | System should not crash; errors must be logged. |

**Steps:**
1. Disconnect MPU6050 physically.
2. Call `gyro_driver_read_gyro(&gyro)` and `gyro_driver_read_accel(&accel)`.
3. Confirm both return an error code.
4. Reconnect MPU6050 and verify normal operation resumes.

---

