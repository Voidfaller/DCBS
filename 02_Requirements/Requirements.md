# Requirements – Dynamic Cup Balancing System (DCBS)

### 1.1 Epics
#### 1.1.1 Adaptive Stabilization
The system shall dynamically regulate the cup tilt angle by automatically controlling servos based on environmental and motion data.

#### 1.1.2 Sensor Integration and Data Processing
The system shall reliably collect, process, and interpret relevant motion data on an embedded platform.

#### 1.1.3 Personalization and Comfort Modeling
The system shall support individual user preferences or scenarios (for example correction speed and maximum tilt tolerance) through configurable parameters.

#### 1.1.4 Reliable and Safe Embedded Architecture
The system shall operate stably, safely, and energy-efficiently in different usage environments (table, desk, education, recreation).

### 1.2 Functional Requirements
#### 1.2.1 Cup Tilt Measurement
The system shall measure or track the cup tilt angle using an appropriate sensor.

#### 1.2.2 Motion Detection
The system shall detect motion of the surface or external disturbances using appropriate sensors.

#### 1.2.3 Real-Time Data Processing
The system shall process sensor values locally with sufficient speed to enable real-time servo control.

#### 1.2.4 Decision Algorithm
The system shall implement an algorithm that determines when and how to actuate the servos based on collected data.

#### 1.2.5 Servo Control
The system shall control servo position and speed based on decision algorithm output and shall implement a PID controller for accurate cup stabilization.

#### 1.2.6 Manual Override
The system shall provide a manual override function that allows users to adjust servo settings, even when automatic control is active.

#### 1.2.7 User Settings
The system shall store and apply user settings such as maximum tilt, correction speed, and sensitivity levels for external motion.

#### 1.2.8 User Interface
The system shall provide a simple user interface (for example buttons and/or a display) for status information and settings adjustment.

#### 1.2.9 Safety and Fault Handling
The system shall implement watchdog timers and detect fault conditions (for example sensor failures and servo faults) to ensure safe operation.

#### 1.2.10 Energy Efficiency
The system shall be energy-efficient, including features such as sleep mode or reduced sampling frequency when no motion or activity is detected.

#### 1.2.11 Load Capacity and Cup Mass Limit
The system shall safely support a total cup load (cup + liquid) up to **1.0 kg** without structural damage, uncontrolled oscillation, or loss of closed-loop control.

### 1.2.12 Cup Physical Specifications
The cup shall meet the following physical requirements:
- **Diameter**: 80 ± 5 mm (at the widest point)
- **Height**: 110 ± 10 mm (from bottom to rim)
- **Cup Mass**: 250 ± 50 g (empty cup without any liquid)
- **Material**: Standard ceramic, glass, or food-safe plastic
- **Rim Design**: Suitable for preventing spillage during dynamic tilting

### 1.2.13 Liquid Specifications
The system shall be designed to handle liquids with the following characteristics:
- **Maximum Volume**: 300 ml (filled to 80% capacity to prevent spillage)
- **Liquid Mass**: Up to 750 g (combined with cup mass, within the 1.0 kg total load limit)
- **Liquid Type**: Water, coffee, tea, or similar beverages with density between 0.9 and 1.1 g/cm³
- **Temperature Range**: 4°C to 85°C (cold to hot beverages)
- **Viscosity**: Low to medium viscosity liquids (similar to water up to thick coffee)

### 1.3 Non-Functional Requirements
#### 1.3.1 Reliability
The system shall operate reliably under different environmental conditions and over extended periods without failure.

#### 1.3.2 Safety
The system shall be safe to use, with no unacceptable risk of damage to the cup, servos, or user.

#### 1.3.3 Usability
The system shall be easy to operate, with an intuitive user interface and clear feedback on status and behavior.

#### 1.3.4 Startup Time
The system shall be fully operational within 10 seconds after power-on.

#### 1.3.5 Maintainability
The system shall be easy to maintain, with modular components and clear documentation for troubleshooting and repair.

#### 1.3.6 Stability Requirement
Under normal operation and disturbances up to **8°**, the system shall keep the cup stable with a residual tilt not exceeding **2°** and no spilling.

#### 1.3.7 Maximum Platform Tilt Angle
The system shall remain controllable and safe up to a platform tilt angle of **12°**. Above this limit, it shall enter a safe mode and notify the user.