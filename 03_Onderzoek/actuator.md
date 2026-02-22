<<<<<<< HEAD
<<<<<<< HEAD
# Actuator Research – Dynamic Cup Balancing System (DBBS)

## 1. Introduction

The Dynamic cup balancing System (DDBS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The actuator converts electrical control signals into mechanical motion. In this project, it must precisely rotate the platform to counteract tilt and maintain a horizontal orientation.
=======
# Actuator Research – Dynamic Comfort Ventilation System (DCVS)

## 1. Introduction

The Dynamic Comfort Ventilation System (DCVS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The system regulates thermal comfort by controlling a fan based on environmental and physiological sensor input. Selecting the appropriate actuator is crucial for accurate and reliable ventilation control.
>>>>>>> c4c4ccd (add actuator and sensor research files)

This document evaluates potential actuator solutions and justifies the final selection based on:

- supports PID
- Interface compatibility with MCU
- Power handling and driver requirements
<<<<<<< HEAD
- control simplicity
=======
>>>>>>> c4c4ccd (add actuator and sensor research files)
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

<<<<<<< HEAD
- Angular position control (not just speed control)
- Compatible with 3.3V or 5V MCU control signals
- PWM controllable for PID integration.
- Low power consumption.
- Budget friendly.
- fast response time
- sufficient torque to support platfrom/object (e.g., glass)
- Platform + glass: 300-600g 

=======
- Able to vary airflow (speed control) for adaptive ventilation
- Compatible with 3.3V or 5V MCU control signals
- PWM controllable for PID integration.
- Low noise for indoor comfort applications.
- Low power consumption.
- Budget friendly.
- Easy integration with RTOS-based system
>>>>>>> c4c4ccd (add actuator and sensor research files)

---

## 4. Actuator Options

<<<<<<< HEAD
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

=======
### 4.1 Option 1 – Standard 12V DC Fan

**Type:** Brushed DC fan  
**Control:** On/off via relay or transistor  
**Voltage:** 12V  
**Current:** 0.2–0.5A  

#### Advantages
- Low cost (~€5)
- Readily available
- Simple integration for basic on/off control

#### Disadvantages
- No native speed control without additional electronics
- PWM control requires external driver circuit
- Limited precision for PID regulation

**Conclusion:**  
Suitable for simple ventilation but lacks integrated speed control. Would require extra components for PWM and PID support.

---

### 4.2 Option 2 – PWM-Controlled DC Fan (Selected)

**Type:** Brushed DC fan  
**Control:** PWM (0–100%)  
**Voltage:** 5–12V  
**Current:** 0.2–0.6A  
**Cost:** ~€10 

#### Advantages
- Supports smooth speed variation
- Fully compatible with MCU PWM output
- Easy to integrate with PID algorithm
- Low noise versions available
- Widely used and documented

#### Disadvantages
- Requires proper MOSFET or transistor driver circuit
- Slightly higher cost than basic DC fans

**Conclusion:**  
The PWM-controlled DC fan is selected because it allows precise airflow adjustment, enabling effective PID-based adaptive ventilation while remaining within the budget.

---

### 4.3 Option 3 – Brushless DC (BLDC) Fan

**Type:** BLDC fan  
**Control:** PWM with dedicated driver IC  
**Voltage:** 5–12V  
**Cost:** ~€20–30  

#### Advantages
- High efficiency
- Very low noise
- Long lifespan
- Smooth speed control

#### Disadvantages
- Higher cost
- More complex driver circuitry
- Limited budget compatibility

**Conclusion:**  
Technically superior, but cost and integration complexity make it less suitable for a €100 group budget.

---

## 5. Fan Driver Circuitry

A simple MOSFET-based driver is sufficient:

- Logic-level N-channel MOSFET controlled by MCU PWM pin
- Flyback diode across fan terminals to protect from voltage spikes
- Optional RC filter for smoother PWM signals.
- Current draw <1A, suitable for on-board MOSFETs
- Cost: ~€3–5 per fan

This ensures precise fan speed control and safe operation.


---

## 6. Integration Considerations

- Fan PWM input connected to MCU timer/PWM output
- Fan driver circuitry provides electrical isolation and current handling
- Fan speed is adjusted in real-time via PID control loop.
- Safety considerations: fan cannot exceed rated voltage/current
- The actuator is integrated into the RTOS task that handles temperature control
>>>>>>> c4c4ccd (add actuator and sensor research files)

---

## 8. Conclusion

<<<<<<< HEAD
The micro servo motor is the most suitable actuator for the Stabilization Platform System.
Compared to DC motors and stepper motors, the servo offers the best balance between performance, simplicity, and budget for a single-axis stabilization system.
=======
//titel
>>>>>>> ce60639 (add Sensor research)
=======
The PWM-controlled DC fan with a MOSFET driver provides:

- Smooth speed control for adaptive ventilation
- Compatibility with PID algorithms
- Low cost and easy integration

This actuator selection aligns with system requirements and budget constraints.
>>>>>>> c4c4ccd (add actuator and sensor research files)
