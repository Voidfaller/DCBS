# Touch Driver Module Test Cases

## Overview

This document defines test cases for the touch driver module responsible for:

- Detecting touch input using capacitive sensing
- Handling interrupt driven touch events
- Tracking user activity
- Triggering deep sleep after inactivity

The touch driver ensures low power operation and responsive user interaction in the embedded system.

---

## 1. Initialization Tests

### TEST1 — Successful Initialization

**Description:**  
Verify that the touch driver initializes correctly.

**Steps:**

- Call:

```c
touch_driver_init()

```

**Expected Result:**

- Returns `ESP_OK`
- Touch pad configured correctly
- Interrupt registered
- FSM started
- Activity timer initialized

---

### TEST2 — Double Initialization

**Description:**  
Verify that calling init multiple times is safe.

**Steps:**
- Call `touch_driver_init()` twice

**Expected Result:**

- Second call does not crash
- Returns `ESP_OK` 
- No duplicate ISR registration

---

### TEST3 — Initialization Failure Handling

**Description:**  
Simulate hardware or driver failure during init.

**Steps:**

- Force `touch_pad_init()` to fail

**Expected Result:**

- Function returns error code
- Driver does not continue initialization

---

## 2. Read Function Tests

### TEST4 — Read Touch Value Successfully

**Description:**  
Verify correct reading of touch data.

**Steps:**

- Call `touch_driver_read(&data)`

**Expected Result:**

- Returns `ESP_OK`
- `data.touch_value` contains raw value
- `data.touched` correctly reflects threshold comparison

---

### TEST5 — Null Pointer Input

**Description:**  
Pass invalid argument.

**Steps:**

- Call `touch_driver_read(NULL)`

**Expected Result:**

- Returns `ESP_ERR_INVALID_ARG`
- No crash occurs

---

### TEST6 — Read Error Handling

**Description:**  
Simulate read failure.

**Steps:**

- Force `touch_pad_read()` to fail

**Expected Result:**

- Function returns error
- No invalid data written

---

## 3. Interrupt Handling Tests

### TEST7 — Touch Interrupt Trigger

**Description:**  
Verify ISR handles touch event.

**Steps:**

- Simulate touch interrupt on configured pin : GPIO4 (ESP32)

**Expected Result:**

- ISR updates activity timestamp
- Task notification sent

---

### TEST8 — Non-MaTESThing Interrupt

**Description:**  
Trigger interrupt on unrelated pin.

**Expected Result:**

- No task notification
- No activity update

---

### TEST9 — ISR Task Notification

**Description:**  
Verify task wake-up from ISR.

**Steps:**

- Trigger touch interrupt GPIO4 (touch-pin)

**Expected Result:**

- `touch_driver_task` receives notification
- Task resumes execution

---

## 4. Task Behavior Tests

### TEST10 — Task Creation

**Description:**  
Verify task starts correctly.

**Steps:**

- Call `touch_driver_start()`

**Expected Result:**

- Task created successfully
- Handle is valid

---

### TEST11 — Double Start Protection

**Description:**  
Call start multiple times.

**Expected Result:**

- No duplicate task created
- Returns `ESP_OK`

---

### TEST12 — Touch Detection in Task

**Description:**  
Verify task processes touch correctly.

**Steps:**

- Simulate touch event
- Let task run

**Expected Result:**

- Touch detected
- Activity timestamp updated
- Log message generated.

---

### TEST13 — No Touch Activity

**Description:**  
System runs without touch input.

**Expected Result:**

- Task remains idle
- No false touch detection

---

## 5. Inactivity & Deep Sleep Tests

### TEST14 — Enter Deep Sleep After Timeout

**Description:**  
Verify inactivity triggers sleep.

**Steps:**
- Do not trigger touch for `TOUCH_SLEEP_TIMEOUT_MS`

**Expected Result:**

- `touch_enter_deep_sleep()` called
- System enters deep sleep

---

### TEST15 — Activity Resets Timeout

**Description:**  
Touch before timeout expires.

**Steps:**

- Trigger touch periodically before timeout

**Expected Result:**

- Timer resets
- System does NOT enter deep sleep

---

### TEST16 — Wakeup Configuration

**Description:**  
Verify wakeup source is set.

**Steps:**

- Trigger deep sleep

**Expected Result:**

- Touchpad wakeup enabled
- System can wake on touch

---

## 6. Logging Tests

### TEST17 — Activity Logging Interval

**Description:**  
Verify logging is rate-limited.

**Steps:**

- Generate continuous touch input

**Expected Result:**

- Logs printed at defined interval
- No log flooding

---

## 7. Concurrency Tests

### TEST18 — Rapid Touch Events

**Description:**  
Simulate high-frequency touch events.

**Expected Result:**

- System remains stable
- No missed or duplicated events

---


## 8. Integration Tests

### TEST19 — Integration with Sleep Module

**Description:**  
Verify interaction with deep sleep system.

**Expected Result:**

- Sleep triggered correctly
- Wakeup works via touch

---

## Summary

This test suite validates:

- Initialization and configuration
- Touch detection and reading
- Interrupt handling
- Task behavior
- Inactivity detection and deep sleep
- Concurrency and stress handling
- Integration with system components