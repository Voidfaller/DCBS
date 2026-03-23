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
- Modular and lightweight for FreeRTOS integration amd ESP32.
  
---


## Hardware Requirements

- **IMU sensor**: MPU6050 
- **Microcontroller**: ESP32 
- Proper wiring for **SDA**, **SCL**, and power supply.

---

## Data strucures

### initialization 

the orientation is initialized to zero.

``` c 
state_estimator_init();

```

### updating orientation 

- call state_estimator_update() at a fixed interval with current gyroscope and accelerometer data.
- integrates gyroscope rates to predict motion.
- calculates tilt angles from accelerometer relative to gravity
- combines both measurements with a complementary filter.

```c 
sttae_estimator_update(&gyro_data, &accel_data, dt);
```

### retrieve orientation 

- get the current estimated orientation for use by the PID controller
  
```c 
orientation_t orientation = state_estimator_get_orientation();
float pitch = orientation.pitch;
float roll  = orientation.roll;

```
## complementary filer:

- the complementary filter blends fast gyroscope response with stable accelerometer data
    orientation = a. (gryo integrated) + (1-a).(accel angle)

- a = 0,98 by default
- gyroscope provides short-term response.
- accelerometer provides long term stability.

## Dependecies:

- gyro_driver: for gyroscope and accelerometer data
- freertos: for task scheduling and timing.


## API reference:

### 1. void_state_estimator_init(void): 
initializes the orientation to zero

### 2. void state_estimator_update(const gyro_data_t *gyro, const accel_data_t *accel, float dt):
updates pitch and roll estimates using gyro and accel data over a timestep dt (S)

### 3. orientation_t / state_estimator_get_orientation(void):
returns the current estimated orientation (pitch and roll)
