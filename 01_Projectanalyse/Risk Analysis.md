# Risk Analysis – Dynamic Cup Balancing System (DCBS)

# 1. Introduction 

The Dynamic Cup Balancing System (DCBS) integrates real time sensing, control algorithms, and mechanical actuation to stabilize a liquid filled cup. Due to the complexity of combining embedded software, hardware components, and physical interaction with liquids, several risks may affect system performance, reliability, and safety.

This section identifies key risks based on the implemented architecture and evaluates their potential impact, likelihood, and mitigation strategies.

## Risk classification 

Risk are categorized into the following domains:

- Sensor and Measurement Risks
- Control system risks
- Real-time and software risks
- hardware and mechanical risks
- safety and user risks

each risk is evaluated in terms of: 

- likelihood: Low, Medium, high
- impact: Low, Medium, High 

## Identified risks:

### R1 Sensor Noise and drift

The MPU6050 sensor combines a gyroscope and accelerometer data. The gyroscope provides smooth data but suffers from drift over time, while the accelerometer is noisy and sensitive to external accelerations. 

- Likelihood: High
- Impact: Medium
- Mitigation: 

1. use complementary filtering: 98% gyroscope and 2% accelerometer.
2. Apply calibration at startup

---

### R2 Incorrect timing assumptions

The system currently uses fixed time steps (dt) for state estimation and PID control. Variations in task scheduling or execution time may lead to incorrect angle estimation and unstable control behavior.

- Likelihood: High
- Impact: high 
- Mitigation:

1. Measure real elasped time using system timers
2. replace fixed dt with dynamic timing.
3. validate timing consistency during testing.
   
---

### R3 PID controller instability 

Improper tuning of PID parameters can result in oscillations, overshoot, or slow response, reducing stabilization effectiveness.

- Likelihood: High
- Impact: high 
- Mitigation:

1. Perform systematic PID tuning
2. Limit controller output
3. identify the correct PID parameters.

---

### R4 real time constraint violations 

The system requires processing within 20 ms. Delays in sensor reading (e.g., I2C timeouts) or task scheduling may violate timing constraints.

- Likelihood: Medium
- Impact: high 
- Mitigation:

1. Use FreeRTOS task prioritization
2. avoid blocking operations
3. monitor execution time 
   
---

### R5 I2C communication failures 

Communication with the MPU6050 via I2C may fail due to noise, bus contention, or timing issues.

- Likelihood: Medium
- Impact: high 
- Mitigation:

1. Use mutex protected I2C access
2. implement retry mechanisms on timeout
3. detect failures and trigger safe behavior 

---

### R6 Liquid spillage (During testing)

Excessive tilt or delayed response may cause liquid to spill, which is a primary failure mode of the system.

- Likelihood: Low
- Impact: high 
- Mitigation:

1. Ensure smooth servo motion
2. optimize control response time 

---

### R7 Watchdog and fault handling limitations:

Although a watchdog mechanism is implemented, it is temporarily disabled during development. This reduces system reliability in case of software failures.

- Likelihood: Medium
- Impact: high 
- Mitigation:

1. Re-enable watchdog in final system
2. improve fault handling before systeem reset

---

### Risk 8 Mechanical instability of the structure:

The mechanical design (e.g., wooden frame and mounting) is not sufficiently rigid and may tilt, vibrate, or even fall over during operation.

- Likelihood: Medium
- Impact: high 
- Mitigation:

1. Reinforce the structure
2. Improve base stability and mounting

---

### R9 Incorrect or misleading display output:

The OLED display may show incorrect or outdated information, such as wrong PID values or state estimator, leading to user confusion or incorrect system tuning.

- Likelihood: Medium
- Impact: Low
- mitigation:

1. Add debugging tools

---

### R10 Incorrect sensor data readings:

The sensor may produce incorrect readings due to calibration errors, noise, or communication issues, leading to incorrect control actions.

- Likelihood: Medium
- Impact: Low
- mitigation:

1. perform proper calibration 
2. Validate sensor data ranges
3. test the sensor before implementing it in the system 

---

### R11 External power supply dependency 

The system currently relies on a laboratory power supply (Stadslab)instead of a standalone internal power source. This may not reflect real world operating conditions and can introduce instability.

- Likelihood: High
- Impact: Medium
- mitigation:

1. Ensure stable voltage regulation 

---

## Conclusion

The DCBS system demonstrates a robust modular architecture with clear separation between sensing, estimation, control, and actuation. However, its performance depends heavily on accurate timing, reliable communication, and proper controller tuning.

By addressing the identified risks particularly timing accuracy, real time behavior, and fault handling the system can achieve reliable and safe operation in real-world conditions.