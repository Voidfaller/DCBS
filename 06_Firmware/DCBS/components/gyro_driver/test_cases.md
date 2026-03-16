# Gyro Driver Test Cases

## Overview

This document describes test cases for the gyro_driver module.  
The purpose of these tests is to verify correct initialization, sensor communication, and data reading from the MPU6050.

Hardware used:
- ESP32
- MPU6050 sensor
- I2C communication

---

# Test Case 1 – Driver Initialization

## Test ID


## Objective
Verify that the gyro driver initializes correctly and the I2C bus starts successfully.

## Steps

1. Connect the MPU6050 to the ESP32.
2. Power on the system.
3. Call: gyro_driver_init();
