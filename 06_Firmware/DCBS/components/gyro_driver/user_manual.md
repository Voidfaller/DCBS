# Gyroscope Driver User Manual

## Overview

This driver provides a simple interface for reading gyroscope and accelerometer data from the MPU6050 sensor using I2C on an ESP32.

The driver initializes the sensor, reads raw motion data, and converts the values into usable physical units.

---

# Hardware Connections

The sensor communicates with the ESP32 using I2C.

| MPU6050 Pin | ESP32 Pin |
|-------------|-----------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

I2C settings: 

- Bus: I2C_NUM_0
- Frequency: 100 kHz
- Address: 0x68

Notes:
- Gyroscope raw values are scalrd by 131.0f (datasheet)
- Accelerometer raw values are scaled by 16384.0f

---

# Driver Architecture

The driver performs the following tasks:

1. Initialize the I2C bus
2. Wake up the MPU6050 sensor
3. Read gyroscope data
4. Read accelerometer data
5. Convert raw values to physical units

---

## Data Structures

### Gyroscope Data
```c 
typedef struct
{
    float gyro_x;
    float gyro_y;
    float gyro_z;
} gyro_data_t;

```

### Accelerometer Data
 ```c

typedef struct
{
    float accel_x;
    float accel_y;
    float accel_z;
} accel_data_t;
```


### combine IMU data
```c
typedef struct
{
    gyro_data_t gyro;
    accel_data_t accel;
} imu_data_t;
```
### API functions 
```c
esp_err_t gyro_driver_init(void);
```

- initializes I2C interface and wakes up the MPU6050
- Must be called before reading sensor data
- returns ESP_OK on succes

### Read Gyroscope and accelerometer data
```c
esp_err_t gyro_driver_read_gyro(gyro_data_t *data);
esp_err_t gyro_driver_read_accel(accel_data_t *data);
```

- reads raw gyroscope/accelerometer values from the sensor and converts to degrees per second
- stores values in the provided Gyro_data_t struct and accel_data_t struct.
- returns ESP_OK on succes 



### intergration with cMake/esp_idf

```c
idf_component_register(
    SRCS "src/gyro_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
)
```

-this allows other components like state_estimator to include gyro_driver.h and acces IMU data in a thread safe manner.


