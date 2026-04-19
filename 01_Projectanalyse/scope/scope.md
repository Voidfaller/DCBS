# Project Scope Bepaling 

#### **Description**

A self stabilizing cup is developed for individuals with hand tremors, such as those caused by Parkinson’s disease. The system detects unwanted movements using sensors, including a gyroscope sensor.
The microcontroller will process the sensor data and adjust the position of the cup using actuators to counteract the tremors, keeping the cup stable. The system will also include an interface to display the current status of the cup such as the angle of tilt and the speed of the actuators. The software will be developed using an embedded Linux, and will include features such as a watchdog timer, sleep mode, hardware interrupts, and software interrupts.

#### **Deliverables**

This project includes the following components:

1. Sensors: a gyroscope is used to measure tilt and motion.
2. Microcontroller: an ESP32 is used to process sensor data and control the system. the controller supports communication protocols sush as I2C/SPI and enables real-time processing
3. Actuator: Two stepper motors are used to adjust the position of the cup. A PID controller determines the motor control signals based on sensor input.
4. Software: Embedded software is developed including:


- Watchdog timer
- sleep mode
- hardware and software interrupts
- Gyro driver
- State estimator
- servo driver
- display driver
- PID controller
  
1. Interface: An Oled display shows system status, sush as tilt angle and PID controller.
2. Documentation: Complete documentation in english, including:

- Description of all system compontents.
- Technical explanation of the system.
- Electrical schematic.
- user manuals
- test cases for all modules

#### **Acceptance criteria**

The system will meet the requirements of the course manual:

- watchdog timer
- sleep modus
- hardware interrupts
- software interrupts
- Use of RTOS / embedded Linux
- communication via I2C and/or SPI
- sensor data acquisition
- System control using a PID controller.
- Actuator control (motors)
- interface with display and user input

The documentation will be completed in English and include a paper with the electrical schematic of the system.


#### **IN scope **

The following items are included within the project scope:

- realization of **Two prototypes** with different approaches.
- Development of a **2-axis** stablization system
- visualization of PID output and system data
- Implementation of a defense mechanism
- research and advice on external power supply usage
- Basic **UX/UI recommendations** for system interaction


#### **Out of scope **

The following items are explicitly exluded from the project scope:

- Intergrated (built in) power supply
- Automated system testing frameworks
- Persistent configuration storage 
- Web application or remote interface

#### **Limitations**

1. **Budget**: A maximum budget of $100 is available for componnents and materials (Bom) .

2. **Time**:  this project must be completed within 8 weeks, including design, implementation, and testing.
3. **Complexity**:he system must remain feasible within the available time, avoiding unnecessary complexity

#### Notes:

- the system is a prototype and not a final consumer ready product
- Performance may depend on hardware limitations and tuning