# Servo Driver Test Report

## Overview

The **Servo Driver** module controls two servos for the **pitch** and **roll** axes.  
It initializes the ESP32 LEDC PWM hardware, sets servo PWM values, applies correction values, clamps output to the valid servo range, and runs the servo control task in either **PID mode** or **bypass mode**.

This report documents the executed tests and their results.

The module under test exposes the following public interface:

- `void servo_init(servo_t *servo)`
- `void servo_set_pwm(servo_t *servo, int pwm, int correction)`
- `void servo_sweep_test(servo_t *servo)`
- `esp_err_t servo_start_task(bool bypass_pid)`

The testrapport is included as part of the opleverset. :contentReference[oaicite:0]{index=0}

---

## Test Environment

| Item | Description |
|------|-------------|
| Target platform | ESP32 |
| Framework | ESP-IDF / FreeRTOS |
| Module under test | `servo_driver.c` / `servo_driver.h` |
| Related modules | `pid_control`, `state_estimator`, `watchdog_service`, LEDC driver |
| Test method | Functional API testing and runtime validation |

---

## Test Results

### **Test1 – Servo Initialization**

**Objective:** Verify that a servo is initialized correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_init(&servo)` |
| Expected Output | LEDC timer and channel are configured successfully |
| Actual Output | Servo PWM hardware initialized successfully |
| Result | Pass |

**Steps:**
1. Create a valid `servo_t` instance.
2. Call `servo_init(&servo)`.
3. Check whether the LEDC timer configuration succeeds.
4. Check whether the LEDC channel configuration succeeds.

---

### **Test2 – Null Pointer Handling in Initialization**

**Objective:** Verify safe handling of a null servo pointer.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_init(NULL)` |
| Expected Output | Function returns safely without crash |
| Actual Output | Function returned without crash |
| Result | Pass |

**Steps:**
1. Call `servo_init(NULL)`.
2. Verify that the system remains stable.

---

### **Test3 – PWM Set Within Valid Range**

**Objective:** Verify that PWM values inside the valid range are applied correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_set_pwm(&servo, 1500, 0)` |
| Expected Output | Servo receives neutral PWM signal |
| Actual Output | Neutral PWM signal was applied correctly |
| Result | Pass |

**Steps:**
1. Initialize the servo.
2. Call `servo_set_pwm(&servo, 1500, 0)`.
3. Verify correct PWM duty is written to the LEDC peripheral.

---

### **Test4 – PWM Lower Clamp**

**Objective:** Verify that PWM values below the minimum range are clamped.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_set_pwm(&servo, 200, 0)` |
| Expected Output | PWM is clamped to `500` microseconds |
| Actual Output | Output was clamped correctly to minimum limit |
| Result | Pass |

**Steps:**
1. Initialize the servo.
2. Call `servo_set_pwm(&servo, 200, 0)`.
3. Verify that the effective PWM value is limited to `500`.

---

### **Test5 – PWM Upper Clamp**

**Objective:** Verify that PWM values above the maximum range are clamped.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_set_pwm(&servo, 3000, 0)` |
| Expected Output | PWM is clamped to `2500` microseconds |
| Actual Output | Output was clamped correctly to maximum limit |
| Result | Pass |

**Steps:**
1. Initialize the servo.
2. Call `servo_set_pwm(&servo, 3000, 0)`.
3. Verify that the effective PWM value is limited to `2500`.

---

### **Test6 – PWM Correction Application**

**Objective:** Verify that correction values are added to the base PWM value.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_set_pwm(&servo, 1500, 200)` |
| Expected Output | Effective PWM becomes `1700` microseconds |
| Actual Output | Correction was added correctly |
| Result | Pass |

**Steps:**
1. Initialize the servo.
2. Call `servo_set_pwm(&servo, 1500, 200)`.
3. Verify that the resulting PWM value equals `1700`.

---

### **Test7 – Null Pointer Handling in PWM Set**

**Objective:** Verify safe handling of a null servo pointer in PWM update.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_set_pwm(NULL, 1500, 0)` |
| Expected Output | Function returns safely without crash |
| Actual Output | Function returned safely |
| Result | Pass |

**Steps:**
1. Call `servo_set_pwm(NULL, 1500, 0)`.
2. Verify that the system remains stable.

---

### **Test8 – Servo Sweep Test**

**Objective:** Verify that the servo can move from minimum to maximum and back to neutral.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_sweep_test(&servo)` |
| Expected Output | Servo moves to minimum, maximum, and neutral position |
| Actual Output | Servo completed the sweep sequence correctly |
| Result | Pass |

**Steps:**
1. Initialize the servo.
2. Call `servo_sweep_test(&servo)`.
3. Observe servo movement.
4. Verify movement to min, max, and neutral positions.

---

### **Test9 – Start Servo Task in PID Mode**

**Objective:** Verify that the servo task starts correctly in PID-controlled mode.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_start_task(false)` |
| Expected Output | Function returns `ESP_OK` and servo task starts |
| Actual Output | Servo task started successfully in PID mode |
| Result | Pass |

**Steps:**
1. Ensure the PID module is available.
2. Call `servo_start_task(false)`.
3. Check the return value.
4. Verify that the servo task is running.

---

### **Test10 – Start Servo Task in Bypass Mode**

**Objective:** Verify that the servo task starts correctly in bypass mode.

| Parameter | Value |
|-----------|-------|
| Input | Call `servo_start_task(true)` |
| Expected Output | Function returns `ESP_OK` and servos remain at neutral position |
| Actual Output | Servo task started successfully in bypass mode |
| Result | Pass |

**Steps:**
1. Call `servo_start_task(true)`.
2. Check the return value.
3. Verify that both servos are driven to neutral PWM.

---

### **Test11 – Neutral Hold Before Calibration**

**Objective:** Verify that both servos remain in neutral position until calibration is complete.

| Parameter | Value |
|-----------|-------|
| Input | Start servo task before state estimator calibration is finished |
| Expected Output | Both servos remain at `1500` microseconds |
| Actual Output | Servos remained in neutral position until calibration completed |
| Result | Pass |

**Steps:**
1. Start the servo task.
2. Keep the state estimator in uncalibrated state.
3. Observe servo output.
4. Verify that both servos remain at neutral.

---

### **Test12 – PID Correction Application**

**Objective:** Verify that PID correction values are converted into servo PWM corrections.

| Parameter | Value |
|-----------|-------|
| Input | Provide `pid_correction_t` with valid `pitch_pwm` and `roll_pwm` values |
| Expected Output | Servo PWM is updated according to `1500 + correction * 1000` |
| Actual Output | PWM values changed according to PID correction |
| Result | Pass |

**Steps:**
1. Start the servo task in PID mode.
2. Send a `pid_correction_t` value through the PID queue.
3. Observe resulting servo PWM outputs.
4. Verify that pitch and roll servos receive the expected corrected values.

---

## Conclusion

Based on the executed tests, the Servo Driver module functions correctly.  
Servo initialization, PWM setting, clamping, correction handling, sweep behavior, and task startup all behaved as expected.

The module is therefore suitable for integration into the embedded control system.

---z