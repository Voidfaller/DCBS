# Actuator Research – Dynamic Comfort Ventilation System (DCVS)

## 1. Introduction

The Dynamic Comfort Ventilation System (DCVS) is developed by a team of five students as part of the TINLab Embedded Systems course.

The system regulates thermal comfort by controlling a fan based on environmental and physiological sensor input. Selecting the appropriate actuator is crucial for accurate and reliable ventilation control.

This document evaluates potential actuator solutions and justifies the final selection based on:

- supports PID
- Interface compatibility with MCU
- Power handling and driver requirements
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

- Able to vary airflow (speed control) for adaptive ventilation
- Compatible with 3.3V or 5V MCU control signals
- PWM controllable for PID integration.
- Low noise for indoor comfort applications.
- Low power consumption.
- Budget friendly.
- Easy integration with RTOS-based system

---

## 4. Actuator Options

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

---

## 8. Conclusion

The PWM-controlled DC fan with a MOSFET driver provides:

- Smooth speed control for adaptive ventilation
- Compatibility with PID algorithms
- Low cost and easy integration

This actuator selection aligns with system requirements and budget constraints.