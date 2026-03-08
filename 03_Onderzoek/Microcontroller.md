# Microcontroller Selection — Justification and Methodology

## 1. Objective

The objective of this analysis is to select a suitable microcontroller (MCU) for the **Dynamic Cup Balancing System (DCBS)**.

The MCU must support real-time stabilization of a cup platform using orientation data from an inertial measurement unit (IMU) and generate control signals for **two servo actuators**. In addition, the MCU must satisfy the mandatory technical constraints of the TINLab Embedded Systems project while remaining compatible with the system’s hardware budget.

The outcome of this analysis is a technically justified MCU platform selection based on explicit functional requirements, system constraints, and development feasibility.

---

# 2. System Overview

The Dynamic Cup Balancing System consists of the following subsystems:

| Subsystem | Function |
|-----------|----------|
| MPU6050 IMU | Measures platform orientation (roll and pitch) |
| Microcontroller | Processes sensor data and executes control algorithms |
| 2 × Servo Motors | Actively stabilize the cup platform |
| Display | Provides system feedback and status |
| Buttons / Input | Allows user configuration and manual override |

System control pipeline:
MPU6050 → Orientation estimation → Control algorithm → PWM signals → 2 servos


The MCU acts as the central processing unit responsible for:

- Sensor acquisition  
- Orientation estimation and filtering  
- Closed-loop control execution  
- PWM signal generation for servo actuators  
- User interface management  
- Fault detection and safety handling  
- Task scheduling under an RTOS  

---

# 3. Project Constraints

## 3.1 Technical Constraints

The project specification requires the embedded system to demonstrate the following capabilities:

- Hardware watchdog timer  
- Sleep mode functionality  
- Hardware and software interrupts  
- RTOS usage  
- Communication via I²C and/or SPI  
- Sensor acquisition and processing  
- Closed-loop control (PID or similar)  
- Actuator control  
- Display and user interface integration  

These constraints directly influence the selection of the MCU platform.

---

## 3.2 Budget Constraint

The total Bill of Materials (BoM) is limited to **€100 per group**, including:

- Microcontroller platform  
- Sensors  
- Actuators  
- Display and user interface hardware  
- Power electronics  
- Supporting circuitry  

The MCU must therefore provide sufficient functionality while maintaining a low cost to allow budget allocation for other system components.

---

# 4. Requirement-Derived MCU Capabilities

Based on the system architecture and requirements specification, the following MCU capabilities are required.

---

## 4.1 Communication Interfaces

The selected IMU sensor (**MPU6050**) communicates via **I²C**.

Additionally, the system includes a display and user interface elements.

Therefore the MCU must provide:

- At least **one I²C peripheral**  
- Optional **SPI support** for display modules  
- Sufficient **GPIO pins with interrupt capability**  

---

## 4.2 Real-Time Control Capability

The stabilization system requires deterministic periodic control execution.

Expected system timing:

| Function | Frequency |
|----------|-----------|
| IMU sampling | ~100 Hz |
| Orientation estimation | ~100 Hz |
| Control loop execution | ~100 Hz |
| Servo update rate | ~50 Hz |

The MCU must therefore support:

- Hardware timers  
- Interrupt-based scheduling  
- RTOS-compatible task management  

---

## 4.3 Actuator Control

The system uses **two servo motors** to stabilize the cup platform.

Therefore the MCU must provide:

- At least **two PWM-capable outputs**  
- Stable timer peripherals capable of generating servo control signals  

Standard hobby servos operate using PWM control pulses at approximately **50 Hz**, meaning the MCU must ensure consistent timing but does not require high computational power.

---

## 4.4 User Interface and Configuration

The system provides a user interface for configuration and manual override.

The MCU must support:

- GPIO inputs for user buttons  
- Display interfacing  
- Runtime configuration management  
- Persistent storage of user settings such as:
  - maximum tilt tolerance
  - correction speed
  - sensitivity levels

---

## 4.5 Safety and Fault Handling

The system must operate safely and detect abnormal conditions.

The MCU must therefore support:

- Hardware watchdog timer  
- Interrupt handling for fault conditions  
- Safe mode handling logic  
- Sensor and actuator error detection  

---

## 4.6 Low-Power Operation

The course specification requires support for energy-efficient operation.

The MCU must therefore provide:

- Sleep modes  
- Timer or interrupt-based wake-up mechanisms  

---

# 5. Memory Requirements

The memory requirements are estimated based on the expected software architecture.

The software stack includes:

- RTOS kernel and scheduler  
- Peripheral drivers (I²C, PWM, GPIO)  
- MPU6050 driver and sensor processing  
- Orientation estimation algorithms  
- Control loop implementation  
- Display interface and UI logic  
- Configuration storage  
- Fault detection and safe mode handling  

---

## 5.1 Estimated Flash Requirement

| Software Component | Estimated Flash |
|--------------------|-----------------|
| RTOS kernel | 40–80 kB |
| Peripheral drivers | 40–100 kB |
| Sensor driver and filtering | 10–30 kB |
| Control algorithm | 10–30 kB |
| UI and display code | 20–80 kB |
| Application logic and margin | 40–100 kB |

Estimated total Flash requirement:

> **160–420 kB**

Therefore the practical baseline requirement is:

> **Flash ≥ 512 kB**

---

## 5.2 Estimated RAM Requirement

| Software Component | Estimated RAM |
|--------------------|---------------|
| RTOS task stacks | 16–40 kB |
| Driver buffers | 8–24 kB |
| Sensor and control variables | 4–12 kB |
| UI buffers | 4–24 kB |
| Application headroom | 8–24 kB |

Estimated RAM requirement:

> **40–124 kB**

Practical baseline requirement:

> **RAM ≥ 64 kB**

---

# 6. Baseline MCU Platform Specification

An MCU platform is considered eligible if it satisfies the following requirements:

| Capability | Requirement Source |
|------------|-------------------|
| 32-bit architecture | RTOS usage |
| Hardware watchdog | Project constraint |
| Sleep mode support | Project constraint |
| At least one I²C peripheral | MPU6050 integration |
| PWM-capable timer outputs | Two-servo control |
| Interrupt-capable GPIO | User interface |
| Non-volatile storage | Firmware + settings |
| ≥512 kB Flash (preferred) | Software footprint |
| ≥64 kB RAM | RTOS operation |
| Practical development board | Implementation feasibility |
| Compatible with €100 BoM | Budget constraint |

---

# 7. Shortlisted MCU Platforms

Based on the baseline specification, the following MCU platforms are considered suitable.

---

## 7.1 Candidate A — ESP32

**Example board:** ESP32 DevKit V1 (ESP32-WROOM-32)

| Specification | Value |
|---------------|-------|
| Architecture | Dual-core Xtensa LX6 |
| Flash | Typically 4 MB |
| RAM | ~520 kB SRAM |

### Characteristics

- Significantly exceeds memory requirements  
- Multiple PWM channels for servo control  
- Multiple I²C and SPI peripherals  
- Strong RTOS compatibility  
- Large development ecosystem  
- Very low board cost  
- Integrated wireless capability for future extensions  

---

## 7.2 Candidate B — STM32F446RE

**Example board:** Nucleo-64 (STM32F446RE)

| Specification | Value |
|---------------|-------|
| Architecture | ARM Cortex-M4F |
| Flash | 512 kB |
| RAM | 128 kB |

### Characteristics

- Hardware floating point unit  
- Mature industrial ecosystem  
- Strong timer and PWM peripherals  
- Excellent RTOS compatibility  

---

## 7.3 Candidate C — ATSAMD51

**Example board:** ATSAMD51 Development Board

| Specification | Value |
|---------------|-------|
| Architecture | ARM Cortex-M4F |
| Flash | 512 kB |
| RAM | 192 kB |

### Characteristics

- Hardware floating point unit  
- Good peripheral architecture  
- Suitable for real-time control applications  

---

## 7.4 Candidate D — RP2040

**Example board:** Raspberry Pi Pico

| Specification | Value |
|---------------|-------|
| Architecture | Dual-core Cortex-M0+ |
| Flash | External QSPI |
| RAM | 264 kB |

### Characteristics

- Very low board cost  
- Flexible peripheral system  
- Large maker ecosystem  

---

# 8. Weighted Evaluation Matrix

Scoring scale:

| Score | Meaning |
|------|---------|
| 1 | Poor |
| 2 | Below Average |
| 3 | Adequate |
| 4 | Strong |
| 5 | Excellent |

| Criterion | Weight | ESP32 | STM32F446RE | ATSAMD51 | RP2040 |
|-----------|--------|-------|-------------|----------|--------|
| Memory Margin | 0.25 | 5 | 4 | 5 | 4 |
| Peripheral Richness | 0.20 | 5 | 5 | 4 | 3 |
| RTOS Suitability | 0.20 | 5 | 5 | 4 | 3 |
| Ecosystem & Documentation | 0.15 | 5 | 5 | 3 | 4 |
| Power Management | 0.10 | 4 | 4 | 4 | 3 |
| Cost Impact | 0.10 | 5 | 4 | 3 | 5 |

---

## Weighted Scores

### ESP32

(5×0.25) + (5×0.20) + (5×0.20) + (5×0.15) + (4×0.10) + (5×0.10)  
= 1.25 + 1.00 + 1.00 + 0.75 + 0.40 + 0.50  
= **4.90**

### STM32F446RE

(4×0.25) + (5×0.20) + (5×0.20) + (5×0.15) + (4×0.10) + (4×0.10)  
= 1.00 + 1.00 + 1.00 + 0.75 + 0.40 + 0.40  
= **4.55**

### ATSAMD51

(5×0.25) + (4×0.20) + (4×0.20) + (3×0.15) + (4×0.10) + (3×0.10)  
= 1.25 + 0.80 + 0.80 + 0.45 + 0.40 + 0.30  
= **4.00**

### RP2040

(4×0.25) + (3×0.20) + (3×0.20) + (4×0.15) + (3×0.10) + (5×0.10)  
= 1.00 + 0.60 + 0.60 + 0.60 + 0.30 + 0.50  
= **3.60**


---

# 9. Final Selection

Based on the weighted evaluation matrix, the **ESP32** achieves the highest overall score.

### Key reasons for selection

The ESP32 is selected because it:

- Provides **large memory headroom** for RTOS-based applications  
- Supports **I²C communication** required by the MPU6050 sensor  
- Provides **multiple PWM outputs** required for controlling two servos  
- Includes required project features such as watchdog timers, interrupts, and sleep modes  
- Offers **excellent cost efficiency** within the €100 BoM constraint  
- Has a **large software ecosystem**, reducing development risk  

Although the STM32F446RE is also a strong candidate, the ESP32 offers a better balance between development accessibility, memory margin, and cost for this project.

Therefore the selected MCU platform for the **Dynamic Cup Balancing System (DCBS)** is:

> **ESP32 DevKit V1 (ESP32-WROOM-32 module)**

---

# 10. Hardware Architecture Implications

Selecting the ESP32 leads to the following minimum hardware interfaces:

| Interface | Purpose |
|-----------|---------|
| I²C bus | MPU6050 communication |
| 2 × PWM outputs | Servo control |
| GPIO inputs | Buttons / manual override |
| Display interface | User feedback |
| 3.3 V rail | MCU and sensor supply |
| 5–6 V rail | Servo power supply |
| Shared ground | System reference |