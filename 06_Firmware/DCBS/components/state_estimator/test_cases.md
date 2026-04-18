# State Estimator Test Cases

## Module Overview

The **State Estimator** module calculates the platform orientation (pitch and roll) using data (gyroscope and accelerometer).  
It applies a **complementary filter** to provide smooth and accurate orientation estimates for PID control.

---

# State Estimator Test Cases: 

## Constants

- Complementary filter coefficient: `ALPHA = 0.98`
- Update interval: `dt = 0.01s`
- Conversion: `RAD_TO_DEG = 57.2958`

---

## 1. Initialization Tests

### TC1 — Initial Orientation

**Description:**  
Verify that the estimator initializes with zero orientation.

**Steps:**

1. Call `state_estimator_init()`
2. Call `state_estimator_get_orientation()`

**Expected Result:**

- `pitch == 0.0`
- `roll == 0.0`

---

## 2. Gyroscope Integration Tests

### TC2 — Pitch Integration

**Description:**  
Verify that pitch is correctly updated using gyro Y-axis.

**Input:**

- `gyro_y = 10 deg/s`
- `dt = 0.01s`

**Expected Calculation:**

pitch = 0 + (10 * 0.01) = 0.1°

**Expected Result:**

- `pitch ≈ 0.1`

---

### TC3 — Roll Integration

**Description:**  
Verify that roll is correctly updated using gyro X-axis.

**Input:**
- `gyro_x = 20 deg/s`
- `dt = 0.01s`

**Expected Result:**
- `roll ≈ 0.2`

---

## 3. Accelerometer Angle Calculation

### TC4 — Flat Orientation

**Description:**  
Device is flat (aligned with gravity).

**Input:**
- `accel_x = 0`
- `accel_y = 0`
- `accel_z = 1`

**Expected Result:**
- `pitch ≈ 0°`
- `roll ≈ 0°`

---

### TC5 — 90° Pitch

**Description:**  
Device tilted forward 90°.

**Input:**
- `accel_x = 1`
- `accel_y = 0`
- `accel_z = 0`

**Expected Result:**
- `pitch ≈ 90°`

---

### TC6 — 90° Roll

**Description:**  
Device tilted sideways 90°.

**Input:**
- `accel_x = 0`
- `accel_y = 1`
- `accel_z = 0`

**Expected Result:**
- `roll ≈ 90°`

---

## 4. Complementary Filter Tests

### TC7 — Gyro Dominance

**Description:**  
Verify that gyro dominates short term updates.

**Input:**

- Previous pitch = 10°
- Gyro update → 11°
- Accelerometer = 0°


**Expected Calculation:**:

pitch = 0.98 * 11 + 0.02 * 0 = 10.78°



**Expected Result:**:

- `pitch ≈ 10.78°`

---

### TC8 — Accelerometer Correction

**Description:**  
Verify that accelerometer slowly corrects drift. (Using complementary filter)

**Input:**

- Gyro drifting away from true value
- Accelerometer stable at 0°

**Expected Result:**

- Pitch gradually converges to 0°

---

## 5. Stability Tests

### TC9 — Constant Input Stability

**Description:**  
Provide constant input over time.

**Input:**

- `gyro = 0`
- `accel = (0, 0, 1)`

**Expected Result:**

- Orientation remains stable at 0°
- No drift over multiple iterations

---

### TC10 — Noise Resistance

**Description:**  
Simulate noisy accelerometer data.

**Input:**
- Small random noise on accel values

**Expected Result:**

- Output remains stable 
- No large oscillations

---

## 6. Edge Cases

### TC11 — Zero Input

**Input:**

- All sensor values = 0

**Expected Result:**
- No crash
- Output remains finite : non Nan

---

### TC12 — Extreme Values

**Input:**

- Very high gyro values (e.g. 1000 deg/s)

**Expected Result:**

- Output updates correctly
- No overflow or instability

---

### TC13 — Null Pointer Safety

**Description:**  
Pass NULL to update function.

**Expected Result:**

- Should be handled safely
- No crash

---

## 7. Integration with Queue

### TC14 — Queue Output

**Description:**  
Verify that orientation is written to queue.

**Steps:**

1. Start state estimator task
2. Send IMU data to `gyro_queue_handle`
3. Read from `state_estimator_queue_handle`

**Expected Result:**

- Orientation values are updated and available in queue

---

## 8. Timing Behavior

### TC15 — Fixed Time Step

**Description:**  
Verify consistent updates with `dt = 0.01s`

**Expected Result:**

- Integration behaves consistently across iterations

---

## Notes

- Hardware is not required; tests can use simulated IMU data

---

## Summary

This test suite validates:

- Initialization
- Gyro integration
- Accelerometer angle calculation
- Complementary filter behavior
- Stability and robustness
- Edge case handling
- Queue integration
