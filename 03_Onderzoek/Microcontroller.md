# Microcontroller Selection — Justification and Methodology

## Objective

The objective of this analysis is to select a suitable microcontroller (MCU) for the **Dynamic Comfort Ventilation System (DCVS)**.

The MCU must satisfy:

- The defined DCVS functional and non-functional requirements
- The mandatory technical constraints of the TINLab Embedded Systems project
- The project hardware budget limitation

The outcome of this analysis is a technically justified MCU class and shortlist, supported by explicit selection criteria.

---

## Project Constraints

### Technical Constraints

The embedded system must demonstrate the following capabilities:

- Hardware watchdog timer  
- Sleep mode  
- Hardware and software interrupts  
- Use of an RTOS or embedded Linux  
- Communication via I²C and/or SPI  
- Sensor acquisition  
- Closed-loop control (preferably PID)  
- Actuator control  
- Interfacing with a display and physical buttons or touchscreen 

These constraints are mandatory for assessment and therefore directly influence MCU selection.

---

### Budget Constraint

The total Bill of Materials (BoM) is limited to **€100 per group**.  

This budget includes:

- MCU  
- Sensors  
- Actuator and driver circuitry  
- Display and input components  
- Power electronics and supporting hardware  

The MCU platform must therefore be cost-efficient and leave sufficient budget for other mandatory components.

---

## Requirement-Derived MCU Criteria

This section translates the defined DCVS requirements and project constraints into concrete MCU capabilities.

### Communication Interfaces

Based on:

- Sensor acquisition requirements  
- Mandatory I²C/SPI communication constraint  
- Display interfacing requirement  

The MCU must provide:

- At least one I²C and/or SPI peripheral  
- Sufficient GPIO with interrupt capability  
- Optional ADC support (depending on final sensor choice)

---

### Real-Time Control Capability

Based on:

- Real-time data processing requirement  
- Decision algorithm requirement  
- PID regulation requirement  
- RTOS requirement  

The MCU must support:

- Hardware timers for periodic control execution  
- Interrupt handling suitable for deterministic scheduling  
- RTOS compatibility  

No specific clock frequency is assumed at this stage.  
Performance sizing will be performed later.

---

### Actuator Control

Based on:

- Fan speed regulation requirement  

The MCU must provide:

- PWM-capable timer output(s)  
- Stable GPIO outputs  

PWM is required to achieve speed control functionality.

---

### Safety and Reliability

Based on:

- Safety and fault-handling requirements  
- Mandatory watchdog constraint  

The MCU must include:

- Hardware watchdog timer  
- Reliable reset behavior  

---

### Low-Power Operation

Based on:

- Energy-efficiency requirement  
- Mandatory sleep mode constraint  

The MCU must provide:

- At least one sleep mode  
- Interrupt- or timer-based wake-up mechanism  

---

### Memory and Storage

Based on:

- RTOS usage  
- UI implementation  
- PID implementation  
- User settings storage  

The MCU must provide:

- Internal non-volatile memory (Flash)  
- Sufficient RAM for RTOS tasks and control variables  

Exact memory sizing will be quantified in a later section.

---

## Selection Strategy

The MCU will be selected using a structured evaluation process.

1. A minimum baseline specification will be derived from the criteria defined in Section 3.  
   Any MCU that does not satisfy all baseline capabilities will be excluded.

2. Candidate MCU families that meet the baseline will be shortlisted.

3. The shortlisted candidates will be compared using a weighted evaluation matrix based on:

   - Technical suitability (timers, peripherals, memory capacity)
   - Cost impact within the €100 BoM constraint
   - RTOS ecosystem maturity and documentation quality
   - Peripheral richness and future expandability
   - Power management capabilities

Each criterion will be assigned a relative weight reflecting its importance to the DCVS project.

The MCU with the highest overall evaluation score, while remaining within the BoM constraint, will be selected.

The final decision will therefore balance:
- Functional compliance
- Technical robustness
- Development feasibility
- Cost efficiency

## Baseline MCU Platform Specification (Mandatory)

An MCU is considered eligible only if it satisfies all of the following:

| Capability | Requirement Source |
|------------|--------------------|
| 32-bit MCU | RTOS usage + real-time control |
| Hardware watchdog | Mandatory project constraint |
| Sleep mode support | Mandatory project constraint + energy efficiency |
| I²C and/or SPI exposed on board | Mandatory communication constraint |
| Hardware timers | Periodic control loop execution |
| Interrupt-capable GPIO available on accessible pins | Mandatory hardware/software interrupt requirement |
| Internal Flash | User settings storage |
| On-board programming/debug support | Development feasibility within 7 weeks |
| Board cost compatible with €100 total BoM | Budget constraint |


## Baseline Resource Assumptions

Because the final sensor and display selection is not yet fixed, resource margins are used to avoid rework and architectural limitations later in development.

- **Flash:** ≥ 256 kB  
- **RAM:** ≥ 64 kB  

These margins are considered sufficient for:

- RTOS kernel and core services  
- Peripheral drivers (I²C, SPI, PWM, GPIO, ADC)  
- PID / control algorithm implementation  
- Basic UI state handling  
- Non-volatile storage of user settings  

This sizing provides implementation headroom without significantly increasing costs.

---

## Shortlist of Candidate MCUs to Compare

Based on the mandatory baseline specification defined above, the following MCU families remain eligible for evaluation.

These candidates will now be compared on secondary selection criteria such as memory margin, peripheral richness, power efficiency, ecosystem maturity, and cost impact within the €100 BoM constraint.

---

### Candidate A — STM32F446RE (STMicroelectronics)

**Example board:** Nucleo-64 (STM32F446RE)  
**Core:** Cortex-M4F  
**Flash / RAM:** 512 kB / 128 kB  

**Matrix-Relevant Characteristics:**
- Exceeds memory baseline with significant margin  
- Multiple advanced timers with PWM capability  
- Multiple I²C / SPI peripherals  
- Hardware FPU (beneficial for PID calculations)  
- Large ecosystem and RTOS support  

---

### Candidate B — MCXC041 (NXP)

**Example board:** MCXC041 Evaluation Board  
**Core:** Cortex-M33  
**Flash / RAM:** 256 kB / 64 kB  

**Matrix-Relevant Characteristics:**
- Meets baseline memory requirements (≥256 kB Flash, ≥64 kB RAM)  
- Cortex-M33 architecture suitable for RTOS-based control  
- Integrated hardware watchdog and low-power modes  
- Timers with PWM capability for actuator control  
- I²C and SPI peripherals available  
- Modern architecture with industrial positioning   

---

### Candidate C — ATSAMD51 (Microchip)

**Example board:** ATSAMD51 Development Board  
**Core:** Cortex-M4F  
**Flash / RAM:** 512 kB / 192 kB  

**Matrix-Relevant Characteristics:**
- Exceeds baseline memory requirements  
- Hardware FPU  
- Sufficient timers and PWM channels  
- Clean peripheral architecture  
- Moderate ecosystem maturity  

---

### Candidate D — TM4C123 (Texas Instruments)

**Example board:** TM4C123 LaunchPad  
**Core:** Cortex-M4F  
**Flash / RAM:** 256 kB / 32 kB  

**Matrix-Relevant Characteristics:**
- Meets Flash baseline  
- RAM below preferred 64 kB safety margin  
- Strong timer and PWM support  
- Mature and stable platform  
- Lower memory headroom compared to other candidates  

---

### Candidate E — RP2040 (Raspberry Pi)

**Example board:** Raspberry Pi Pico  
**Core:** Dual-core Cortex-M0+  
**Flash / RAM:** External Flash / 264 kB SRAM  

**Matrix-Relevant Characteristics:**
- RAM exceeds baseline  
- No internal Flash (external QSPI required)  
- No hardware FPU  
- Lower-performance Cortex-M0+ core  
- Very low board cost  
- Strong maker ecosystem, moderate industrial positioning  

---

## Weighted Evaluation Matrix

To ensure objective selection, each shortlisted candidate is evaluated using a weighted scoring model.

Scoring scale:
- 1 = Poor
- 2 = Below Average
- 3 = Adequate
- 4 = Strong
- 5 = Excellent

Weights reflect the relative importance of each criterion for the DCVS project.

| Criterion | Weight | STM32F446RE | MCXC041 | ATSAMD51 | TM4C123 | RP2040 |
|------------|--------|--------------|----------|-----------|----------|---------|
| Memory Margin | 0.25 | 5 | 3 | 5 | 2 | 4 |
| Peripheral Richness (Timers, I²C/SPI, PWM) | 0.20 | 5 | 4 | 4 | 4 | 3 |
| RTOS Suitability | 0.20 | 5 | 4 | 4 | 4 | 3 |
| Power Management | 0.15 | 4 | 4 | 4 | 3 | 3 |
| Ecosystem & Documentation | 0.10 | 5 | 3 | 3 | 3 | 4 |
| Cost Impact | 0.10 | 4 | 5 | 4 | 5 | 5 |

### Weighted Scores

STM32F446RE:
(5×0.25) + (5×0.20) + (5×0.20) + (4×0.15) + (5×0.10) + (4×0.10)  
= **4.75**

MCXC041:
(3×0.25) + (4×0.20) + (4×0.20) + (4×0.15) + (3×0.10) + (5×0.10)  
= **3.85**

ATSAMD51:
(5×0.25) + (4×0.20) + (4×0.20) + (4×0.15) + (3×0.10) + (4×0.10)  
= **4.35**

TM4C123:
(2×0.25) + (4×0.20) + (4×0.20) + (3×0.15) + (3×0.10) + (5×0.10)  
= **3.35**

RP2040:
(4×0.25) + (3×0.20) + (3×0.20) + (3×0.15) + (4×0.10) + (5×0.10)  
= **3.55**

## Final Selection

Based on the weighted evaluation matrix, the STM32F446RE achieves the highest overall score.

Key reasons for selection:

- Significant memory headroom beyond baseline
- Strong timer and PWM capabilities for actuator control
- Hardware FPU supporting efficient PID implementation
- Mature RTOS ecosystem and documentation
- Balanced cost within the €100 BoM constraint

Although other candidates meet the baseline requirements, the STM32F446RE provides the best overall balance between technical robustness, implementation margin, ecosystem maturity, and cost efficiency.

Therefore, the selected MCU platform for the DCVS project is:

> **STM32F446RE (Nucleo-64 development board)**