# DCBS state Estimator Module – User Manual

## overview

The State Estimator module calculates the orientation of the system (pitch and roll) using sensor data from the gyroscope and accelerometer (MPU6050).


It uses a **complementary filter** to combine:
- Gyroscope data (fast, but drifts over time)
- Accelerometer data (stable, but noisy)

This results in a stable and responsive estimation of the platform tilt.


---

## Data Flow

Gyro + Accel → State Estimator → Pitch/Roll → PID Controller

---

