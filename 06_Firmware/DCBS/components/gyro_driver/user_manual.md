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

---

# Driver Architecture

The driver performs the following tasks:

1. Initialize the I2C bus
2. Wake up the MPU6050 sensor
3. Read gyroscope data
4. Read accelerometer data
5. Convert raw values to physical units

---

# API Reference

## gyro_driver_init()

Initializes the I2C bus and wakes up the MPU6050 sensor
