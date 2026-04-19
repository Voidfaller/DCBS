# Watchdog Module Test Cases

## Overview

This document defines test cases for the watchdog module responsible for:

- Monitoring system tasks
- Detecting unresponsive components
- Triggering recovery actions : reset

The watchdog ensures system reliability in a real time embedded environment.

---

## 1. Initialization Tests

### TC1 — Successful Initialization

**Description:**  
Verify that the watchdog initializes correctly.

**Steps:**

1. Call `watchdog_init(timeout_s)`

**Expected Result:**

- Returns `ESP_OK`
- Watchdog system is active
- Internal structures initialized

---

### TC2 — Double Initialization

**Description:**  
Verify that multiple init calls are safe.

**Steps:**

1. Call `watchdog_init()` twice

**Expected Result:**

- Second call returns `ESP_OK`
- No duplicate initialization
- No memory leak

---

### TC3 — Invalid Timeout

**Description:**  
Test invalid timeout values.

**Input:**

- `timeout_s = 0` or negative

**Expected Result:**

- Returns `ESP_ERR_INVALID_ARG`
- Watchdog not started

---

## 2. Task Registration Tests

### TC4 — Register Task Successfully

**Description:**  
Register a task with the watchdog.

**Steps:**

1. Call `watchdog_register("TaskA", 5)`

**Expected Result:**

- Valid handle returned
- Task added to watchdog list

---

### TC5 — Register Multiple Tasks

**Description:**  
Register multiple tasks.

**Expected Result:**

- All tasks tracked independently
- Unique handles assigned

---

### TC6 — Register with Invalid Parameters

**Description:**  
Pass invalid inputs.

**Input:**

- NULL name
- Invalid timeout

**Expected Result:**

- Returns `WATCHDOG_INVALID_HANDLE`
- No task registered

---

### TC7 — Maximum Task Limit

**Description:**  
Register tasks up to system limit.

**Expected Result:**

- Registration succeeds until limit

---

## 3. Feed Tests

### TC8 — Feed Watchdog Successfully

**Description:**  
Feed a registered task.

**Steps:**

1. Register task
2. Call `watchdog_feed(handle)`

**Expected Result:**

- Timer resets for that task
- No timeout triggered

---

### TC9 — Feed with Invalid Handle

**Description:**  
Use an invalid or unregistered handle.

**Expected Result:**

- Function safely ignores or returns error
- No crash

---

### TC10 — Multiple Feeds

**Description:**  
Feed watchdog repeatedly.

**Expected Result:**

- No side effects
- Timer consistently reset

---

## 4. Timeout Behavior

### TC11 — Task Timeout Detection

**Description:**  
Do not feed watchdog within timeout.

**Steps:**

1. Register task with 2s timeout
2. Wait >2s without feeding

**Expected Result:**

- Watchdog detects timeout
- Error/log triggered
- Recovery action executed (e.g., reset or callback)

---

### TC12 — Multiple Task Timeout

**Description:**  
Multiple tasks with different timeouts.

**Expected Result:**

- Each task monitored independently
- Only expired tasks trigger timeout

---

### TC13 — Feed Just Before Timeout

**Description:**  
Feed watchdog just before expiration.

**Expected Result:**

- No timeout triggered
- Timer resets correctly

---

## 5. Unregister Tests

### TC14 — Unregister Task

**Description:**  
Remove task from watchdog.

**Steps:**

1. Register task
2. Call `watchdog_unregister(handle)`

**Expected Result:**

- Task removed from monitoring
- No further timeout checks

---

### TC15 — Unregister Invalid Handle

**Description:**  
Attempt to unregister invalid handle.

**Expected Result:**

- Safe failure
- No crash

---

## 6. System Behavior

### TC16 — Watchdog Recovery Action

**Description:**  
Verify system reaction to timeout.

**Expected Result:**

- Reset OR callback triggered
- System enters safe state

---

### TC17 — Watchdog with Idle System

**Description:**  
No tasks registered.

**Expected Result:**

- Watchdog does not trigger unnecessarily
- System remains stable

---

## 7. Concurrency Tests

### TC18 — Multi-Task Feeding

**Description:**  
Multiple tasks feed watchdog concurrently.

**Expected Result:**

- No race conditions
- All feeds processed correctly

---

### TC19 — Feed During Timeout Check

**Description:**  
Feed occurs while watchdog checks timeouts.

**Expected Result:**

- No data corruption
- Correct behavior maintained

---

## 8. Stress Tests

### TC20 — Rapid Register/Unregister

**Description:**  
Repeatedly add/remove tasks.

**Expected Result:**

- No memory leaks
- Stable operation

---

### TC21 — High Frequency Feeding

**Description:**  
Feed watchdog at very high rate.

**Expected Result:**

- No performance degradation
- No overflow or instability

---

## 9. Integration Tests

### TC22 — Integration with Gyro Task

**Description:**  
Ensure gyro task feeds watchdog correctly.

**Expected Result:**

- No timeout under normal operation

---

### TC23 — Integration with State Estimator

**Description:**  
Verify estimator task interaction.

**Expected Result:**

- Task remains monitored
- No unintended resets

---

### TC24 — System-Wide Failure Simulation

**Description:**  
Simulate task freeze.

**Expected Result:**

- Watchdog detects failure
- System recovery triggered

---

## 10. Edge Cases

### TC25 — Handle Reuse

**Description:**  
Unregister and re-register task.

**Expected Result:**

- New handle works correctly
- No stale references



---

## Notes

- Use logging to verify timeout events

---

## Summary

This test suite validates:

- Initialization and configuration
- Task registration and tracking
- Feeding behavior
- Timeout detection and recovery
- Concurrency and stress handling
- Integration with system tasks