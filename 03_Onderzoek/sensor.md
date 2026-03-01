# Sensor Research – Dynamic Comfort Ventilation System (DCVS)

## 1. Introduction

The Dynamic cup balancing System (DDBS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The system aims to regulate thermal comfort by controlling a fan based on environmental and physiological sensor input. To enable adaptive and intelligent control, reliable and accurate sensors are required.

This document evaluates potential sensor solutions and justifies the final selection based on:

- Accuracy
- Communication compatibility (I2C requirement)
- Power consumption
- Real-time capability
- Integration complexity
- Budget constraints

---
## 2. Budget Constraint

The total Bill of Materials (BoM) is limited to **€100 per group**.

This budget includes:

- Microcontroller (MCU)
- Sensors
- Actuator and driver circuitry
- Display and input components
- Power electronics and supporting hardware

Because the team consists of five members, the selected components must be cost-efficient while still meeting technical and course requirements. The MCU platform and sensors must leave sufficient financial margin for mandatory components such as:

- Display and user interface
- Power regulation
- PCB and supporting electronics

Sensor selection must therefore balance performance and cost.

---
## 3. Sensor Requirements

The following requirements must be fulfilled by the chosen sensors:

- Operating temperature range: 15°C – 35°C.
- Accuracy: ±1.0°C or better for ambient sensing
- Compatible with 3.3V microcontrollers (e.g., STM32)
- Support I2C and SPI communication
- Low power consumption
- Sampling frequency ≥ 1 Hz
- Suitable for integration in an RTOS-based embedded system

---

# 4. Ambient Environment Sensor Analysis

## 4.1 Option 1 – SW520D Tilt Switch

**Type:** Digital temperature and humidity sensor  
**Accuracy:** ±0.5°C (temperature)  
**Interface:** Proprietary single-wire protocol  

### Advantages
- Low cost
- Easy to use

### Disadvantages
- Only detects tilt threshold.
- No angle measurement
- no suitable for PID control.

**Conclusion:**  
The tilt switch is unsuitable because the stabilization system requires continuous angle measurement, not just binary tilt detection.

---

## 4.2 Option 2 – MPU6050 (Selected)

**Type:** 3 axis accelerometer /3 axis gyproscope (6 DOF IMU) 
**Degrees of freedom:** 6-axis (Acc/Gyro) 
**On board processing:** DMP (digital Motion Processor)
**Interface:** I2C  
**Price:** LOW
**Operating voltage** : 3.3V

### Advantages
- Measures acceleration and angular velocity
- I2C support
- Low power consumption
- suitable for sensor fusion.
- low cost
- high update rate (> 1kHz internal sampling)

### Disadvantages
- requires filtering
- raw accelerometer data affected by vibration

**Conclusion:**  
The MPU6050 is selected because it provides continuous angle information and rotational velocity. By combining accelerometer and gyroscope data using Kalman filter, accurate and stable tilt estimation can be achieved.

---


## 4.3 Option 3 - BNO055

**Type:** 9 DOF absolute orientation sensor 
**Interface:** I2C
**Degrees of freedom** : 9-axis (Acc/Gyro/Mag)
**Price** : High

### Advantages
- Provides direct angle output
- high accuracy

### Disadvantages
- higher cost
- higher power consumption

**Conclusion:** 
While technically superior, the BNO055 exceeds the project requirements and budget constraints. Therefore, it was not selected. The MPU6050 is the most logical choice due to its balance of cost and extensive library support
---


# 5. Final Sensor Selection

| Selected Sensor       | Interface | Resolution | update rate |
|------------------------|----------------|-----------|----------|
| MPU6050    | I2C        | <1 (filtered)       | > 50HZ   |


 the selected sensor:

- Operate at 3.3V
- Support I2C communication
- is compatible with STM32 microcontrollers.
- Support real-time sampling.

---

# 6. Integration Considerations

- Pull-up resistors (typically 4.7kΩ) are required on SDA and SCL (DATASHEET).
- Sampling frequency: 100Hz.
- Raw sensor data is noisy. To achieve "adaptive stabilization", the software must implement a filter to combine accelerometer and gyroscope data.
- Power suply: Ensure clean 3.3V.

---

# 8. Conclusion

The BME280 and MLX90614 sensors provide a balanced combination of environmental and physiological monitoring capabilities.

By including humidity measurement through the BME280, the DCVS system can implement more advanced comfort modeling instead of relying solely on temperature.

Both sensors support I2C communication, ensuring compatibility with the embedded architecture and fulfilling project requirements.