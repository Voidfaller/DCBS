# System User Manual

## Overview

This system is a modular embedded control system designed for orientation stabilization.  
It consists of the following components:

- Gyro Driver
- State Estimator
- PID Controller
- Servo Driver
- Watchdog Service
- Display Driver

Each module operates independently but communicates through FreeRTOS queues.

---

# 1. Gyro Driver

## Description

The Gyro Driver interfaces with the MPU6050 sensor over I2C.  
It provides real-time gyroscope and accelerometer data.

## Features

- Reads gyroscope (°/s)
- Reads accelerometer (g)
- Automatic retry on I2C timeout
- Continuous sampling via FreeRTOS task
- Queue-based data output

## API

### Initialize

```c
esp_err_t gyro_driver_init(void);
```

### start driver task

 ```c

esp_err_t gyro_driver_start(void);

```

### read gyro data 

```c
esp_err_t gyro_driver_read_gyro(gyro_data_t *data);
```

### Read accel data

```c
esp_err_t gyro_driver_read_accel(accel_data_t *data);

```

### output 

- Queue: gyro_queue_handle
- Data type: imu_data_t


# 2. State estimator


## Description

The State Estimator calculates device orientation (pitch and roll) using sensor fusion.

## Algorithm

- Gyroscope integration (short-term accuracy)
- Accelerometer correction (long-term stability)
- Complementary filter (fusion)

## API
### Start estimator:

```c
esp_err_t state_estimator_start(void);

```

### update : internal use
```c
void state_estimator_update(imu_data_t *data, float dt);
```

### Get orientation

```c 
orientation_t state_estimator_get_orientation(void);
```

### output:

- Queue: state_estimator_queue_handle
- data: pitch and roll : degrees

# 3. PID controller

## Description

The PID Controller stabilizes the system based on orientation error.

## Functionality

- Computes correction from pitch and roll.
- outputs control signal for servos.

## API

### start controller:

```c
esp_err_t pid_control_start(void);

```

### Input:

- Orientation from state estimator

### output:

- Control signal

### Notes:

- Requires tuning: kp,ki,kd 
- runs continuously in its own task

# 4. Servo driver

## Description 
Controls servo motors to physically stabilize the system.

## features: 

-PWM-based control
-Pitch stabilization support
-Optional PID bypass mode : for testing 

## API
### start pitch servo 

```c 

esp_err_t servo_stabilizer_start_pitch(void);
```


### enable/disable PID bypass:

```c
void servo_set_pitch_pid_bypass(bool enable);
```

### Behavior:

- uses PID output or direct orientation (bypass mode)
- updates servo position continuously 

### notes:

- requires correct GPIO configuration (hardware ontwerp)
- PID bypass useful for debugging!

# 5. Watchdog service 

## Description:
Monitors system tasks and ensures they remain responsive.

## features

- task registration
- timeout detection
- system recovery

## API

### initialize 

```c
void watchdog_init(int timeout_seconds);
```

### register task

```c
watchdog_handle_t watchdog_register(const char *name, int timeout_seconds);
```


### Feed watchdog

```c
void watchdog_feed(watchdog_handle_t handle);
```

### Behavior

- Each task must feed watchdog periodically
-Failure triggers recovery action

## Notes

- Critical for system stability
- Used in gyro, PID, and other tasks



# System flow:

Gryo driver -> State estimator -> PID controller -> Servo driver 
-> Display driver
.> sleep module
-> watchdog : monitors all active components 



# Startup sequence:

1. Initialize watchdog
2. Initialize config manager
3. Initialize I2C
4. Start gyro driver
5. Start state estimator
6. Start PID controller
7. Start servo driver
8. Start display driver

# Check

## NO sensor data

- check I2C initialization
- verify MPU6050 connection
- check device address: 0x68

## Unstable orientation

- Tune complementary filer
- check sensor noise 
- verify mounting orientation

## Servo not responding

- Check GPIO configuration 
- verify PWM output
- Disable PID bypass for testing

## Watchdog reset

- ensure all tasks call watchdog_feed()
- increase timeout if needed.

# Notes!

- All modules use FreeRTOS
- communication via queues
- designed for real time operation 