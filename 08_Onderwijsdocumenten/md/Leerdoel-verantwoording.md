Leerdoelen Verantwoording
=========================
In dit document worden de leerdoelen van TINLES03 Embedded Systems verantwoord.
# Leerdoelen
| Leerdoel | Producten | Verantwoording |
|----------|-----------|----------------|
| 1. [Analyseren] Je kunt op basis van contact met de stakeholder(s), het probleem definiëren, de opdracht specificeren, onderzoeksvragen opstellen, de scope bepalen en passende requirements opstellen. | <ul><li>Problem Description</li><li>Stakeholder Analysis</li><li>Requirements</li><li>Scope</li></ul> | - |
| 2. [Analyseren] Je kunt op basis van literatuur- en experimenteel onderzoek onderbouwde conclusies trekken. | <ul><li>Actuator Onderzoek</li><li>Microcontroller Onderzoek</li><li>Sensor Onderzoek</li><li>English Paper</li></ul> | - |
| 3. [Adviseren] Je kunt aanbevelingen doen over keuzes en een eventueel vervolg van de opdracht gebaseerd op je analyse en resultaten. |<ul><li>Aanbevelings Rapport</li><li>English Paper</li></ul>| - |
| 4. [Ontwerpen] Je kunt bepalen welke ontwerpen met betrekking tot structuur, architectuur en dynamische aspecten van hardware, software, netwerkinfrastructuur en/of interacties relevant zijn voor de eigen opdracht en deze ontwerpen opstellen. |<ul><li>Software Architectuur Diagram</li><li>Systeem architectuur diagram</li><li>Mechanisch Ontwerp</li><li>Hardware Schematic</li></ul>| - |
| 5. [Realiseren] Je kunt alle relevante testen ontwikkelen, uitvoeren en hierover rapporteren. |<ul><li>Test Plannen</li><li>Test rapporten</li></ul> | - |
| 6. [Realiseren] Je kunt programmacode schrijven die voldoet aan gegeven kwaliteitseisen en rekening houdt met onverwachte situaties (exceptions). |<ul><li>Source Code</li></ul> | - |
| 7. [Realiseren] Je kunt een werkend prototype realiseren in overeenstemming met de ontwerpen en de kwaliteitseisen. | <ul><li>PO Meetings</li><li> Presentatie</li></ul> | - |
| 8. [Manage & Control] Je kunt bepalen welke documentatie relevant is voor de overdracht aan de gebruikers en deze opstellen. | <ul><li>Leerdoel Verantwoording</li><li>Gebruikers Handleiding</li><li>ReadMe's in de code</li></ul> | - |
| 9. [Manage & Control] Je kunt je project organiseren en waar nodig tussentijds aanpassen aan gegeven budget en tijd. | <ul><li>Jira</li><li>PO Meetings</li></ul> | - |
| 10. [Professional Skills] Je kunt voor de opdracht relevante kennis verwerven en overbrengen binnen teamverband. | <ul><li>Daily Standups</li><li>PO Meetings</li></ul> | - |

## Technische bewijsmatrix (gevraagde onderwerpen)

- watchdog timer
	- [Watchdog implementatie (broncode)](../../06_Firmware/DCBS/components/watchdog_service/src/watchdog_service.c)
	- [Watchdog testrapport](../../05_Testen/Testplannen/docx/testreport_Watchdog.docx)

- sleep modus
	- [Deep sleep en touch wake-up (touch driver)](../../06_Firmware/DCBS/components/touch_driver/src/touch_driver.c)
	- [Touch driver testrapport](../../05_Testen/Testplannen/docx/testreport_Touch_driver.docx)

- hardware interrupts
	- [Touch interrupt handler (ISR)](../../06_Firmware/DCBS/components/touch_driver/src/touch_driver.c)
	- [Touch component flowchart](../../04_Ontwerpen/Software/component%20flowcharts/touch_driver/touch_driver.svg)

- software interrupts
	- [Taaknotificatie vanuit ISR naar taak (event-driven afhandeling)](../../06_Firmware/DCBS/components/touch_driver/src/touch_driver.c)
	- [Touch driver ontwerpdiagram](../../04_Ontwerpen/Software/component%20flowcharts/touch_driver/touch_driver.drawio)
	- [RTOS gebruik van vTaskDelay](../../06_Firmware/DCBS/components/watchdog_service/src/watchdog_service.c) technisch gezien een vorm van software interrupt (taakplanning)

- gebruik van RTOS / embedded Linux
	- [FreeRTOS gebruik in main](../../06_Firmware/DCBS/main/main.c)
	- [FreeRTOS taken in watchdog service](../../06_Firmware/DCBS/components/watchdog_service/src/watchdog_service.c)

- communicatie via I2C en/of SPI
	- [I2C wrapper implementatie](../../06_Firmware/DCBS/components/I2C_wrapper/src/I2C_wrapper.c)
	- [I2C initialisatie in main](../../06_Firmware/DCBS/main/main.c)

- uitlezen van een sensor
	- [Sensoronderzoek](../../03_Onderzoek/docx/sensor.docx)
	- [Gyro driver testrapport](../../05_Testen/Testplannen/docx/testreport_Gyro_driver.docx)

- regelen van een systeem (bij voorkeur met PID-controller)
	- [PID testrapport](../../05_Testen/Testplannen/docx/testreport_pid.docx)
	- [PID flowchart](../../04_Ontwerpen/Software/component%20flowcharts/pid_control/pid_control.svg)

- aansturen van een actuator (motor driver, relais of iets dergelijks. Een LEDje is geen actuator.)
	- [Servo driver testrapport](../../05_Testen/Testplannen/docx/testreport_servo.docx)
	- [Actuatoronderzoek](../../03_Onderzoek/docx/actuator.docx)

- interfacing met een display (LCD, OLED of anders; denk aan Nixie-tubes, meerdere LEDs, wijzertje) en knoppen of touchscreen
	- [Display driver testrapport](../../05_Testen/Testplannen/docx/testreport_display.docx)
	- [Touch driver testrapport](../../05_Testen/Testplannen/docx/testreport_Touch_driver.docx)
	- [Display flowchart](../../04_Ontwerpen/Software/component%20flowcharts/display_driver/display_driver.svg)