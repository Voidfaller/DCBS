# Project Scope Bepaling 

#### **Description**
We are going to build a self stabilizing cup targeted to people with hand tremors such as Parkinson's Disease. The cup will use sensors such as a gyroscope to detect movements caused by the user.
The microcontroller will process the sensor data and adjust the position of the cup using actuators to counteract the tremors, keeping the cup stable. The system will also include an interface to display the current status of the cup such as the angle of tilt and the speed of the actuators. The software will be developed using an embedded Linux, and will include features such as a watchdog timer, sleep mode, hardware interrupts, and software interrupts.

#### **Deliverables**
We will deliver the following components for our project:
1. **Sensors:** We will use **gyroscope** to collect data about the movements caused by the user.
2. **Microcontroller:** We will use a **STM32** to process the sensor data and adjust the position of the cup using actuators to counteract the tremors, keeping the cup stable. This controller supports **I2C/SPI** and embedded Linux.
3. **Actuator:** We will install **2 stepper** motors that can automatically adjust the position of the cup based on the data from the sensors. This will be managed by a **PID-Controller** that adjusts the speed of the actuators based on the sensor data.
4. **Software:** We will develop software using **embedded Linux**. The software will include features such as a watchdog timer, sleep mode, hardware interrupts, and software interrupts.
5. **Interface:** We will develop an interface for an **OLED display** that shows the current status of the cup, such as the angle of tilt and the speed of the actuators.
6. **Documentation:** We will provide documentation for each of the deliverables, including a paper with the electrical schematic of the system in English.

#### **Acceptance criteria**
The system will meet the requirements of the course manual:
"
- watchdog timer
- sleep modus
- hardware interrupts
- software interrupts
- gebruik van RTOS / embedded Linux
- communicatie via I2C en/of SPI
- uitlezen van een sensor
- regelen van een systeem (bij voorkeur met PID-controller)
- aansturen van een actuator (motor driver, relais of iets dergelijks. Een LEDje is geen actuator.)
- interfacing met een display (LCD, OLED of anders; denk aan Nixie-tubes, meerdere LEDs, wijzertje) en knoppen of touchscreen
"
- The documentation will be completed in English and include a paper with the electrical schematic of the system.

#### **Limitations**
1. **Budget**: We have a limited budget of 100 euros for the purchase of components and materials.
2. **Time**: We have a limited time of 8 weeks to complete the project, including design, implementation, and testing.
3. **Complexity**: We need to ensure that the system does not become too complex, so that we can stay within the time limit and make the project feasible for our team.
