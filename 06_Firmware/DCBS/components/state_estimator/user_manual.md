# DCBS state Estimator Module – User Manual

## overview

The State Estimator module calculates the orientation of the system (pitch and roll) using sensor data from the gyroscope and accelerometer (MPU6050).


It uses a **complementary filter** to combine:
- Gyroscope data (fast, but drifts over time)
- Accelerometer data (stable, but noisy)

This results in a stable and responsive estimation of the platform tilt.


---

## Requirements:

- Calculates **pitch** and **roll** in degrees.
- Uses a **complementary filter** for sensor fusion.
- Provides **real-time updates** suitable for PID controllers.
- Modular and lightweight for FreeRTOS integration.
  
---


## Hardware Requirements

- **IMU sensor**: MPU6050 
- **Microcontroller**: ESP32 
- Proper wiring for **SDA**, **SCL**, and power supply.


---

## parameters

|  Parameter |  Description |
|-------------|-----------|
| Gyro | Pointer to gyro_data_t (deg/s) |
| Accel | Pointer to accel_data_t (g) |
| dt | Time elapsed sice last update (S) |

- dt should match our sensor samling interval
- the complementary filter combines gyro and accelerometer for stable orientation


---




