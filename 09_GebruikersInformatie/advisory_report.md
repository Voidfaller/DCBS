# Advisory Report

## 1. Introduction

This report presents a comprehensive evaluation of the developed stabilization system, including its design concept, implementation results, encountered challenges, and recommendations for further improvement. The system is intended to stabilize orientation using sensor input, signal processing, and actuator control, forming a complete embedded control loop.

The primary goal of the project is to develop a functional stabilization mechanism capable of detecting angular motion and compensating for it in real time, thereby maintaining a stable orientation under varying conditions.

---

## 2. Project concept

The Dynamic Cup Balancing System operates as a real time pipeline in which sensor data, estimation, control, and actuation are continuously processed in a cyclic manner. The IMU provides raw gyroscope and accelerometer data at a fixed sampling interval of approximately 10ms.

The state estimator applies a complementary filter to combine short term accurate gyroscope data with long term stable accelerometer data, resulting in a filtered estimate of the platform orientation in terms of pitch and roll. This estimation step is crucial, as raw sensor data is too noisy for direct control usage.

The estimated orientation is then forwarded to the control subsystem. Initially, a PID controller was implemented to calculate corrective actions based on the difference between the desired orientation (setpoint) and the measured orientation. However, during testing it was observed that the PID controller did not consistently improve system stability.

As a result, an alternative open loop approach was evaluated, where control signals are applied directly based on measured orientation (complementary filter). This simplified approach resulted in more stable and predictable behavior under the tested conditions.

Finally, the servo driver converts the control signal into PWM output, which actuates the servo motor to adjust the platform. All modules operate concurrently using FreeRTOS tasks, communicating via queues to ensure non blocking real-time behavior.

The intended architecture can be summarized as follows:

- Sensor input (gyroscope and accelerometer: MPU6050)
- State estimation (orientation calculation using complementary filter)
- Control algorithm (PID controller)
- Actuation (servo motor response)

This approach is commonly used in control systems such as drones, gimbals, and balancing platforms, where accurate and stable feedback control is essential.

---
## 3. Reflection on development process: 7 weeks

The development of the Dynamic Cup Balancing System took place over a period of seven weeks.
In the initial phase of the project, the focus was placed on designing a closed loop stabilization system based on a PID controller.

Following feedback from the project owner (Soufianne Lemkaddem), the initial concept was reconsidered, leading to a redesign of the control approach. This resulted in a delay of approximately two weeks.

In the first phase, the system was tested without a PID controller. The system works correctly and was able to stabilize the cup effectively using gyroscope and accelerometer data with a complementary filter. Based on the requirements for obtaining Tinlab Embedded Systems, the use of PID was required.

During later stages of development, extensive testing was performed comparing PID based control with an open loop approach. Contrary to expectations, the open loop system demonstrated more stable and reliable behavior under the given conditions. This outcome highlighted the importance of practical validation, as theoretical solutions do not always translate directly into effective real-world performance.

In addition to control related challenges, hardware limitations also affected development. The use of temporary wiring instead of a dedicated PCB introduced noise and instability, while reliance on an external laboratory power supply influenced system behavior.

Overall, the development process demonstrated the importance of iterative design, testing, and validation in embedded systems. 


---

## 4. System Evaluation

### 4.1 Control Strategy Testing

The system was tested using two different control approaches: closed loop control using a PID controller and open loop control

In the closed loop configuration, the PID controller calculates corrective outputs based on the error between the desired and measured orientation. In contrast, the open loop configuration applies control signals directly without PID.

### 4.2 Results

Testing revealed that the open loop control system produced more stable and predictable behavior compared to the PID-controlled system. The closed loop system, in its current implementation, introduced instability and did not improve overall performance. (see demo video)

Possible causes for this behavior include:

- Improper PID tuning
- Sensor noise affecting feedback accuracy
- Delay in the control loop
- Insufficient filtering of input signals

### 4.3 Conclusion

Based on the observed results, the open loop control system currently performs better than the closed loop PID implementation. However, this does not imply that closed loop control is inherently inferior, but rather that further tuning and refinement are required for optimal performance. It is advised against further investigating why a closed loop control works better than with a PID controller in the case of our system.

---

## 5. Key Findings

### 5.1 Power Supply Influence

The addition of an external and stable power supply (Laboratory Power 5V) resulted in a noticeable improvement in system performance. This highlights the importance of proper power management in embedded systems.

Improvements included:

- More consistent servo operation
- Elimination of unexpected system resets

---

### 5.2 Hardware Limitations

Several hardware related limitations were identified during testing:

- Use of temporary wiring instead of a structured PCB design
- Lack of mechanical optimization

These factors negatively impact system reliability and repeatability.

---

### 5.3 Mechanical Design

The current setup lacks an ergonomic structure, which affects usability and practical handling. Without a proper mechanical design, including a stable and ergonomic handle, the system cannot be considered user friendly or suitable for real world application.

---

## 6. Recommendations

### 6.1 Further Research

Additional research is required to transform the current prototype into a fully functional and reliable product. This includes both hardware and software improvements.

---

### 6.2 Open Loop Control Optimization

Given the current performance, further investigation into optimizing the open loop control system is recommended. This includes refining the mapping between sensor input and actuator output, as well as improving stability.

---

### 6.3 Closed Loop Control Re-evaluation

Although the PID controller did not perform well in its current state, it is recommended to revisit this approach after addressing the following:

- Proper PID tuning
- Improved sensor filtering

Closed loop control remains essential for advanced stabilization systems and should not be discarded entirely.

---

### 6.4 Power System Design

A dedicated and optimized power system should be implemented, including:

- Stable voltage regulation
- Proper grounding

This will improve overall system reliability and performance.

---

### 6.5 PCB Development

Designing a PCB is strongly recommended to reduce wiring complexity and increase system robustness.

---

### 6.6 Ergonomic Improvements

The implementation of an ergonomic handle is proposed as a potential improvement to enhance usability and user interaction with the system. Such a design could allow for better control and more realistic operation.

However, further investigation is required to determine whether this approach is suitable and beneficial for the current system. Factors such as system weight, balance, vibration, and user interaction should be evaluated before implementation.

---

## 7. Conclusion

The developed system demonstrates a functional proof of concept for orientation stabilization, however, it is not yet ready for practical deployment. While the open loop control system currently provides better performance, further development is required to achieve a fully optimized and reliable solution.

Key areas for improvement include control strategy refinement, hardware design, power stability, and mechanical usability. With these improvements, the system has the potential to evolve into a robust and effective stabilization platform.

---

## 8. Future Work

Future development should focus on:

- Implementation of full axis stabilization
- Hardware integration through PCB design
- using intern power
- Enhancement of system robustness and usability
- Experimental validation under real world conditions