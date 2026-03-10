# Requirements – Dynamic Cup Balancing System (DCBS)

## 1. System Overview
The Dynamic Cup Balancing System (DCBS) is an embedded system designed to stabilize a cup containing liquid by dynamically adjusting a platform using servo actuators.  
The system measures platform motion and cup tilt using inertial sensors and compensates for disturbances through a PID-based control loop.  
It is intended for educational, demonstration, or desk/table usage.

---

## 2. Epics

### E1 – Adaptive Stabilization
The system shall automatically stabilize the cup by adjusting the platform orientation based on measured tilt and detected disturbances.

### E2 – Sensor Integration and Data Processing
The system shall collect motion data from sensors, filter the measurements, and compute tilt estimates for use in the stabilization algorithm.

### E3 – Personalization and User Interaction
The system shall allow users to view system status and configure stabilization parameters.

### E4 – Reliable and Efficient Embedded Architecture
The system shall operate reliably by detecting faults and entering power down states when necessary.

---

## 3. Functional Requirements

### FR1 – Cup Tilt Measurement
The system shall measure the cup tilt angle in two axes (pitch and roll) using an inertial sensor.  
- Measurement frequency: **≥50 Hz**  
- Measurement accuracy: **±1°**

### FR2 – Motion Detection
The system shall detect external disturbances that cause platform tilt or acceleration.  
- Disturbances greater than **1°** shall trigger stabilization actions.

### FR3 – Data Filtering
The system shall apply a filtering algorithm (e.g., Kalman or complementary filter) to sensor data to reduce noise.  
- Filtered tilt estimate update frequency: **≥50 Hz**

### FR4 – Real-Time Data Processing
The system shall process sensor measurements and compute actuator commands within **20 ms** of receiving new sensor data.

### FR5 – PID Control Algorithm
The system shall compute stabilization corrections using a **PID controller** based on measured tilt.  
- PID output shall be used to adjust servo positions smoothly and responsively.

### FR6 – Servo Control
The system shall control servo motors to adjust platform orientation.  
- Update frequency: **≥50 Hz**  
- Servo positions must remain within mechanical limits.

### FR7 – Manual Override
The system shall allow the user to disable automatic stabilization and manually control servo positions.  
- Manual control overrides PID stabilization while active.

### FR8 – User Settings
The system shall allow users to configure stabilization parameters:  
- Correction speed  
- Sensitivity  
- Maximum allowable tilt  
Settings shall persist across power cycles.

### FR9 – User Interface
The system shall provide a user interface with buttons and/or a display.  
The interface shall allow the user to:  
- View current tilt angle  
- View system status  
- Adjust system settings

### FR10 – Safety and Fault Handling
The system shall detect hardware and software faults, including:  
- Sensor communication failure  
- Servo control failure  
- Software execution failure  
Detected faults shall trigger safe behavior.

### FR11 – Watchdog Supervision
The system shall implement a watchdog timer to detect software failures.  
- On timeout, the system shall reset and return to a safe initialization state.

### FR12 – Energy Efficiency
The system shall reduce energy consumption during inactivity.  
- If no motion is detected for **60 seconds**, the system shall reduce sensor sampling and suspend servo movement.  
- Normal operation shall resume automatically when motion is detected.

---

## 4. Non-Functional Requirements

### NFR1 – Startup Time
The system shall enter operational mode within **10 seconds** after power-on.

### NFR2 – Stability Requirement
During disturbances up to **8° platform tilt**, the system shall maintain a residual cup tilt of **≤2°**.

### NFR3 – Maximum Platform Tilt
The system shall remain controllable up to **12° platform tilt**.  
- Exceeding this limit triggers safe mode and user notification.

### NFR4 – Reliability
The system shall operate continuously for **≥8 hours** without failure.

### NFR5 – Usability
The user interface shall provide clear feedback on system status, including tilt, mode, and fault notifications.

### NFR6 – Maintainability
The system architecture shall be modular, separating:  
- Sensor drivers  
- State estimation/filter  
- PID controller  
- Servo drivers  
- User interface  
- Watchdog/fault handling  
Each module shall be independently testable.

---

## 5. System Constraints

### SC1 – Load Capacity
The system shall safely support a total load of **1.0 kg** (cup + liquid) without structural damage or loss of control.

### SC2 – Cup Physical Specifications
The cup shall have:  
- Diameter: **80 ± 5 mm**  
- Height: **110 ± 10 mm**  
- Mass (empty): **250 ± 50 g**  
- Material: ceramic, glass, or food-safe plastic  
- Rim design suitable to prevent spillage

### SC3 – Liquid Specifications
The system shall support liquids with:  
- Maximum volume: **300 ml**  
- Density: **0.9–1.1 g/cm³**  
- Temperature range: **4°C–85°C**  
- Low to medium viscosity (e.g., water to thick coffee)
