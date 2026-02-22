<<<<<<< HEAD
# Actuator Research – Dynamic Cup Balancing System (DBBS)

## 1. Introduction

The Dynamic cup balancing System (DDBS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The actuator converts electrical control signals into mechanical motion. In this project, it must precisely rotate the platform to counteract tilt and maintain a horizontal orientation.

This document evaluates potential actuator solutions and justifies the final selection based on:

- supports PID
- Interface compatibility with MCU
- Power handling and driver requirements
- control simplicity
- Real-time response
- Cost and integration complexity
- Budget constraints

---

## 2. Budget Constraint

The total Bill of Materials (BoM) is limited to **€100 per group**, which includes:

- Microcontroller (MCU)
- Sensors
- Actuator and driver circuitry
- Display and input components
- Power electronics and supporting hardware

As the team consists of five members, the selected actuator must be cost efficient.

---

## 3. Actuator Requirements

The chosen actuator must fulfill the following requirements:

- Angular position control (not just speed control)
- Compatible with 3.3V or 5V MCU control signals
- PWM controllable for PID integration.
- Low power consumption.
- Budget friendly.
- fast response time
- sufficient torque to support platfrom/object (e.g., glass)
- Platform + glass: 300-600g 


---

## 4. Actuator Options

### 4.1 Option 1 – DC Motor + Encoder

**Type:** Brushed DC motor with position feedback
**Control:** PWM+ direction control


#### Advantages
- High speed
- Good torque options
- Suitable for advanced control systems

#### Disadvantages
- Requires encoder for position control
- more complex driver circuit
- more complex software
- higher cost

**Conclusion:**  
Although powerful, a DC motor with encoder adds unnecessary complexity for a single-axis stabilization system.

---

### 4.2 Option 2 – Servo motor (Selected)

**Type:** Closed loop rotary actuator
**Control:** PWM signal (50HZ topical)
example model: SG90 or MG90S micro servo

#### Advantages
- built in position control
- low cost
- fast response time
- simple pwm interface
- compact and lightweight
- sufficient torque for small platform

#### Disadvantages
- limited rotation range
- plastic gear versions have lower durablity
- slight mechanical blackflash

**Conclusion:**  
The servo motor is selected because it provides integrated position control, simple PWM interfacing, sufficient torque, and low system complexity.
it is ideal for our small angle stabilization applications.

---

### 4.3 Option 3 – Stepper motor

**Type:** Incremental step motor
**Control:** Step and direction signals

#### Advantages

- Precise incremental movement
- Good holding torque
- No encoder required

#### Disadvantages
- Higher current consumption
- Generates vibration
- Can lose steps under load
- Larger and heavier
- 
**Conclusion:**  
Stepper motors are suitable for CNC style positioning but are less efficient and over-dimensioned for small angle stabilization.
---

## 5. Micro servo motor

A servo motor is a closed loop actuator consisting of:
- DC motor
- Gear reduction system
- Potentionmeter 
- internal control circuit

the internal controller adjusts the motor position until the desired angle is reached. This means: the microcontroller does not control motor speed directly. It simply commands a target angle.

## 6. Torque consideration

 Since a full cup of coffee can weigh around 300-500g, we need servos with sufficient stall torque 

- SG90: 1,8 kg/cm
- MG996R: 10 kg/cm

Estimated required torque:
Torque = Force * Distance
### Option 1: MG996R

- Torque: 10 kg/cm.
- Gears: Metal
- Pros: Very strong; can easily handle the weight of the gimbal plus a full cup.
- Cons: Larger and heavier; requires a 5V-6V external power supply.

### Option 2: SG90 

-Torque : 1,8 kg/cm
- Gears : plastic
- Pros: tiny, very cheap, runs off USB power
- cons: might struggle with a heavy mug; plastic gears can strip under stress

For better reliability, a metal gear servo (MG996R) is preferred.

---
# 7. Final Actuator Selection

| Parameter         | Selected Actuator     |
|------------------|----------------------|
| Type             | Micro Servo Motor    |
| Control          | PWM                  |
| Voltage          | 5V                   |
| Torque           | ~2 kg·cm (minimum)   |
| Position Control | Built-in             |
| Cost             | Low                  |


---

## 8. Conclusion

The micro servo motor is the most suitable actuator for the Stabilization Platform System.
Compared to DC motors and stepper motors, the servo offers the best balance between performance, simplicity, and budget for a single-axis stabilization system.
=======
//titel
>>>>>>> ce60639 (add Sensor research)
