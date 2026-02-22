<<<<<<< HEAD
# Sensor Research – Dynamic Cup Balancing System (DBBS)

## 1. Introduction

The Dynamic cup balancing System (DDBS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The DBBS requires a sensor capable of measuring orientation (Pitch and Roll) in real time. To satisfy the project requirements, the sensor must support I2C or SPI communication and provide data at a rate suitable for a PID control loop (at least 50-100Hz).
=======
# Sensor Research – Dynamic Comfort Ventilation System (DCVS)

## 1. Introduction

The Dynamic Comfort Ventilation System (DCVS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The system aims to regulate thermal comfort by controlling a fan based on environmental and physiological sensor input. To enable adaptive and intelligent control, reliable and accurate sensors are required.
>>>>>>> ce60639 (add Sensor research)

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

<<<<<<< HEAD
=======
- Fan driver circuitry (e.g., MOSFET + protection)
>>>>>>> ce60639 (add Sensor research)
- Display and user interface
- Power regulation
- PCB and supporting electronics

Sensor selection must therefore balance performance and cost.

---
## 3. Sensor Requirements

The following requirements must be fulfilled by the chosen sensors:

<<<<<<< HEAD
- Angle measurement in at least 1 axis (preferably 2 axes)
- Resolution better than 1°
- Compatible with 3.3V microcontrollers (e.g., STM32)
- Support I2C and SPI communication
- Low power consumption
- Update rate ≥ 50 Hz (for smooth servo control)
=======
- Operating temperature range: 15°C – 35°C.
- Accuracy: ±1.0°C or better for ambient sensing
- Compatible with 3.3V microcontrollers (e.g., STM32)
- Support I2C and SPI communication
- Low power consumption
- Sampling frequency ≥ 1 Hz
>>>>>>> ce60639 (add Sensor research)
- Suitable for integration in an RTOS-based embedded system

---

<<<<<<< HEAD
# 4. Technology Selection

For dynamic stabilization, a 6-axis IMU is the industry standard. It combines:
- Accelerometer: Measures gravity to determine the "down" direction (static accuracy).
- Gyroscope: Measures angular velocity (dynamic accuracy during movement).

## 4.1 Option 1 – SW520D Tilt Switch

**Type:** Digital tilt switch
**Voltage:** 3,3V- 5V
**Interface:** Digital output (HIGH/LOW) 
=======
# 4. Ambient Environment Sensor Analysis

## 3.1 Option 1 – DHT22

**Type:** Digital temperature and humidity sensor  
**Accuracy:** ±0.5°C (temperature)  
**Interface:** Proprietary single-wire protocol  
>>>>>>> ce60639 (add Sensor research)

### Advantages
- Low cost
- Easy to use
<<<<<<< HEAD
- No configuration required

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

# 7. Conclusion

the MPU6050 is the best fit for our requirements. It allows for I2C communication, provides the necessary data for a PID controller, and supports hardware interrupts for waking the system from sleep mode.
=======

### Disadvantages
- Slow sampling rate (~0.5 Hz)
- Timing-sensitive communication
- Not I2C-based.

**Conclusion:**  
The DHT22 does not align with the system requirement to use I2C communication.

---

## 3.2 Option 2 – BME280 (Selected)

**Type:** Environmental sensor (temperature, humidity, pressure)  
**Temperature accuracy:** ±1.0°C  
**Humidity accuracy:** ±3% RH  
**Interface:** I2C or SPI  
**Operating voltage:** 1.8V – 3.6V  

### Advantages
- Measures temperature, humidity, and pressure
- Native I2C support
- Low power consumption
- Compact and widely supported
- Suitable for indoor comfort analysis

### Disadvantages
- Slightly lower temperature accuracy compared to high-precision sensors (e.g., ±0.1°C sensors)

**Conclusion:**  
The BME280 is selected as the ambient environment sensor because it provides both temperature and humidity measurements via I2C.  
Humidity data improves thermal comfort modeling, making the system more intelligent and adaptable compared to using temperature alone.

---

# 5. Skin Temperature Sensor Analysis

Physiological input is required to personalize thermal comfort regulation.

## 4.1 Option 1 – DS18B20 (Contact-Based)

**Accuracy:** ±0.5°C  
**Interface:** 1-Wire  

### Advantages
- Accurate
- Low cost
- Easy implementation
- Can be powered from data line. Power supply range is 3.oV to 5.5V.

### Disadvantages
- Requires physical skin contact
- Affected by airflow and placement
- Less comfortable for users

**Conclusion:**  
Technically viable but less user-friendly.

---

## 4.2 Option 2 – MLX90614 (Selected)

**Type:** Infrared non-contact temperature sensor  
**Accuracy:** ±0.5°C (typical)  
**Interface:** I2C  
**Operating voltage:** 3.3V – 5V  

### Advantages
- Non-contact measurement
- Comfortable for users
- Low cost IR sensor.
- I2C compatible
- Suitable for real-time applications

### Disadvantages
- Sensitive to measurement distance and angle
- Slightly higher cost

**Conclusion:**  
The MLX90614 is selected due to its non-contact measurement method and I2C compatibility. It allows safe and comfortable physiological temperature monitoring.

---

# 6. Final Sensor Selection

| Measurement Type       | Selected Sensor | Interface | Accuracy |
|------------------------|----------------|-----------|----------|
| Ambient Environment    | BME280        | I2C       | ±1.0°C   |
| Skin Temperature       | MLX90614      | I2C       | ±0.5°C   |

Both sensors:

- Operate at 3.3V
- Support I2C communication
- Can share the same I2C bus (different addresses)
- Are compatible with STM32 microcontrollers.
- Support real-time sampling (1–10 Hz).

---

# 7. Integration Considerations

- Both sensors communicate via I2C.
- Pull-up resistors (typically 4.7kΩ) are required on SDA and SCL (DATASHEET).
- Sampling frequency: 1–10 Hz.
- Sensor values will be filtered using a moving average.
- Data is processed in a dedicated RTOS task.
- Sensor outputs serve as input for the PID control algorithm.

---

# 8. Conclusion

The BME280 and MLX90614 sensors provide a balanced combination of environmental and physiological monitoring capabilities.

By including humidity measurement through the BME280, the DCVS system can implement more advanced comfort modeling instead of relying solely on temperature.

Both sensors support I2C communication, ensuring compatibility with the embedded architecture and fulfilling project requirements.
>>>>>>> ce60639 (add Sensor research)
