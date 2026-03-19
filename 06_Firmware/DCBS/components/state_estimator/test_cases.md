# State Estimator Test Cases

## Module Overview

The **State Estimator** module calculates the platform orientation (pitch and roll) using data (gyroscope and accelerometer).  
It applies a **complementary filter** to provide smooth and accurate orientation estimates for PID control.

---

## Test Case Guidelines

- Verify **initialization, update logic, and output correctness**.
- Test with **known IMU inputs** to confirm expected orientation.
- Include **error handling** and edge cases.

---

## Test Cases

### **Test1 – Initialization**

**Objective:** Ensure the state estimator initializes correctly.

| Parameter | Value |
|-----------|-------|
| Input | Call `state_estimator_init()` |
| Expected Output | Pitch = 0, Roll = 0 |
| Notes | Must be called before updates. |

**Steps:**
1. Call `state_estimator_init()`.
2. Retrieve orientation using `state_estimator_get_orientation()`.
3. Verify `pitch` and `roll` are both 0.

---

### **Ttest2 – Static IMU Data**

**Objective:** Confirm correct orientation for stationary sensor.

| Parameter | Value |
|-----------|-------|
| Input | Accelerometer: aligned with gravity, Gyro: 0 deg/s |
| Expected Output | Pitch = 0, Roll = 0 |
| Notes | Small variations due to filter coefficients are acceptable. |

**Steps:**
1. Provide accel readings for flat, stationary orientation (e.g., `accel_z = 1.0g`).
2. Provide gyro readings = 0.
3. Call `state_estimator_update(&gyro, &accel, dt)`.
4. Retrieve orientation and verify pitch and roll are near zero.

---

### **testcase3 – Tilt Simulation**

**Objective:** Verify orientation matches known tilts.

| Parameter | Value |
|-----------|-------|
| Input | Accelerometer: simulate 10 pitch forward, Gyro: 0 deg/s |
| Expected Output | Pitch = 10, Roll = 0 |
| Notes | Complementary filter will blend gyro and accel. |

**Steps:**
1. Compute accelerometer values corresponding to a 10 pitch.
2. Set gyro = 0.
3. Call `state_estimator_update()` repeatedly.
4. Confirm orientation output converges to expected pitch and roll.

---

### **Testcase4 – Gyroscope Integration**

**Objective:** Verify orientation changes over time based on gyro readings.

| Parameter | Value |
|-----------|-------|
| Input | Gyro: 90 deg/s pitch rotation for 0.5 s, Accel: 0 |
| Expected Output | Pitch = 45, Roll = 0 |
| Notes | Orientation should integrate gyro properly over time. |

**Steps:**
1. Set `gyro_pitch = 90 deg/s`, `gyro_roll = 0`.
2. Set `accel` readings to zero or flat.
3. Update state estimator in a loop with `dt = 0.02 s` for 25 iterations.
4. Retrieve orientation and verify pitch =45.

---

### **testcase5 – Complementary Filter Behavior**

**Objective:** Ensure filter blends gyro and accelerometer correctly.

| Parameter | Value |
|-----------|-------|
| Input | Accelerometer indicates 10 tilt, Gyro indicates 0 tilt |
| Expected Output | Orientation moves gradually towards accel value, not instant jump |
| Notes | Filter coefficient (e.g., 0.98) controls responsiveness. |

**Steps:**
1. Provide accel = 10 tilt, gyro = 0.
2. Call `state_estimator_update()` repeatedly.
3. Confirm orientation starts at previous value and gradually converges to 10.

---

