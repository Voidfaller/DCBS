# PID Controller Test Report

## Overview

The **PID Controller** module calculates correction outputs for the **pitch** and **roll** axes based on orientation data from the State Estimator.  
This report documents the executed tests and their results.

The module under test exposes the following public interface:

- `void pid_controller_init(void)`
- `float pid_controller_update(pid_controller_t *pid, float measurement)`
- `esp_err_t pid_control_start(void)`
- `QueueHandle_t pid_control_get_queue_handle(void)`

---

## Test Environment

| Item | Description |
|------|-------------|
| Target platform | ESP32 |
| Framework | ESP-IDF / FreeRTOS |
| Module under test | `pid_control.c` / `pid_control.h` |
| Related module | `state_estimator` |
| Test method | Functional API testing and runtime validation |

---

## Test Results

### **Test1 – PID Initialization**

**Objective:** Verify that the PID controller state is initialized correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_controller_init()` |
| Expected Output | PID controller state variables are reset |
| Actual Output | `integral`, `last_error`, and `filtered_derivative` were reset correctly |
| Result | Pass |

**Steps:**
1. Power on the board.
2. Call `pid_controller_init()`.
3. Inspect the PID controller state.
4. Verify that state variables are reset and ready for use.

---

### **Test2 – Zero Error Output**

**Objective:** Verify that no correction is generated when measurement equals setpoint.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_controller_update(&pid, measurement)` with `measurement = setpoint` |
| Expected Output | Output equals `0` or approximately `0` |
| Actual Output | Output remained `0` |
| Result | Pass |

**Steps:**
1. Initialize a PID controller instance.
2. Set `setpoint` equal to the measurement.
3. Call `pid_controller_update()`.
4. Verify that the returned correction is zero.

---

### **Test3 – Proportional Response**

**Objective:** Verify that the controller output increases when the error increases.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_controller_update()` with increasing error values |
| Expected Output | Output increases proportionally until limit is reached |
| Actual Output | Output increased with larger error values |
| Result | Pass |

**Steps:**
1. Initialize a PID controller instance.
2. Set `ki = 0` and `kd = 0`.
3. Apply increasing measurement offsets.
4. Verify that the returned output increases accordingly.

---

### **Test4 – Integral Build-Up**

**Objective:** Verify that the integral term accumulates during a constant error.

| Parameter | Value |
|-----------|-------|
| Input | Repeated calls to `pid_controller_update()` with constant error |
| Expected Output | Output gradually increases due to integral action |
| Actual Output | Output increased over time |
| Result | Pass |

**Steps:**
1. Initialize a PID controller instance.
2. Apply a constant error repeatedly.
3. Call `pid_controller_update()` multiple times.
4. Verify that the controller output gradually increases.

---

### **Test5 – Integral Limit**

**Objective:** Verify that the integral term is limited to prevent windup.

| Parameter | Value |
|-----------|-------|
| Input | Repeated large constant error |
| Expected Output | Integral term remains within configured limit |
| Actual Output | Integral term was clamped correctly |
| Result | Pass |

**Steps:**
1. Initialize a PID controller instance.
2. Apply a large constant error for an extended period.
3. Call `pid_controller_update()` repeatedly.
4. Verify that the integral term does not exceed its limit.

---

### **Test6 – Output Saturation**

**Objective:** Verify that the controller output is clamped within the allowed range.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_controller_update()` with very large error |
| Expected Output | Output remains between `-1.0` and `1.0` |
| Actual Output | Output was clamped correctly |
| Result | Pass |

**Steps:**
1. Initialize a PID controller instance.
2. Apply a measurement that creates a large error.
3. Call `pid_controller_update()`.
4. Verify that the output stays within the defined bounds.

---

### **Test7 – Null Pointer Handling**

**Objective:** Verify safe handling of an invalid PID pointer.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_controller_update(NULL, measurement)` |
| Expected Output | Function returns `0.0f` without crashing |
| Actual Output | Function returned `0.0f` safely |
| Result | Pass |

**Steps:**
1. Call `pid_controller_update(NULL, 0.0f)`.
2. Observe the return value.
3. Verify that no crash or undefined behavior occurs.

---

### **Test8 – Start PID Control Module**

**Objective:** Verify that the PID control module starts correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_control_start()` |
| Expected Output | Function returns `ESP_OK` |
| Actual Output | PID control task started successfully |
| Result | Pass |

**Steps:**
1. Ensure the system is initialized.
2. Call `pid_control_start()`.
3. Check the return value.
4. Verify that the PID control task is running.

---

### **Test9 – Queue Handle Availability**

**Objective:** Verify that the PID output queue can be retrieved.

| Parameter | Value |
|-----------|-------|
| Input | Call `pid_control_get_queue_handle()` |
| Expected Output | Valid queue handle is returned after startup |
| Actual Output | Queue handle was returned successfully |
| Result | Pass |

**Steps:**
1. Start the PID module with `pid_control_start()`.
2. Call `pid_control_get_queue_handle()`.
3. Verify that the returned handle is not `NULL`.

---

### **Test10 – Pitch and Roll Correction Output**

**Objective:** Verify that correction values for both axes are produced correctly.

| Parameter | Value |
|-----------|-------|
| Input | Orientation updates for pitch and roll |
| Expected Output | `pid_correction_t` contains valid `pitch_pwm` and `roll_pwm` values |
| Actual Output | Both fields were updated correctly |
| Result | Pass |

**Steps:**
1. Start the PID control module.
2. Provide orientation input through the State Estimator.
3. Read the latest `pid_correction_t` from the PID queue.
4. Verify that both `pitch_pwm` and `roll_pwm` contain valid correction values.

---

## Conclusion

Based on the executed tests, the public API of the PID Controller module functions correctly.  
Initialization, update logic, error handling, module startup, queue access, and correction output all behaved as expected.

The module is therefore suitable for integration into the full embedded control system.

---