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

gyro_driver_init() → ESP_ERR_INVALID_STATE 


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

output:

1. gyro_x ≈ -126 / 131 = -0.96
2. gyro_y ≈ -0.96
3. gyro_z ≈ -0.96

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

output:

1. accel_x = 1.0
2. accel_y = 0.0
3. accel_z = 0.0
   
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


s_mpu6050_dev = NULL
gyro_driver_init() 

---

### **test5 – Task level tests intergration**
#### step 1
except:

gyro_driver_start() → ESP_OK
task handle != NULL
queues created
watchdog registered

#### Step 2

Setup:
1. Gyro task running
2. Mock I2C always OK

Except:
1. Queue wordt gevuld
2. no watchdog timeout
3. period ~10ms.

---

### Test mocks:

#### I2C wrapper:

i2c_wrapper_read_reg()
i2c_wrapper_write_reg_u8()
i2c_wrapper_add_device()

#### FreeRTOS:
xQueueCreate
xQueueOverwrite
vTaskDelay
xTaskCreate

#### ESP timer:

esp_timer_get_time()
