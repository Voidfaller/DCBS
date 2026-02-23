
# Project Scope Bepaling 


#### **Omschrijving**
Wij gaan een adaptieve ventilatorsysteem ontwerpen dat automatisch de luchtstroom aanpast op basis van de omgevingstemperatuur. Het systeem zal gebruik maken van sensoren om gegevens te verzamelen en een microcontroller om de ventilatoren aan te sturen.  

#### **Deliverables**
1. **Sensoren**: We zullen gebruik maken van **temperatuursensoren** om gegevens te verzamelen over de omgeving.
2. **Microcontroller**: We zullen een [Geen idee] gebruiken om de gegevens van de sensoren te verwerken en de ventilatoren aan te sturen op basis van de verzamelde gegevens. Deze controller ondersteunt **I2C/SPI**.
3. **Actuator**: We zullen ventilatoren installeren die automatisch kunnen worden aangepast op basis van de gegevens van de sensoren. Hiermee beheren we de **PiD-Controller** die de snelheid van de ventilatoren aanpast op basis van de temperatuurgegevens. Daarnaast ook het aansturen van een **actuator**.
4. **Software**: We zullen software ontwikkelen met **RTOS / embedded  Linux**. De software maakt gebruik van 
- watchdog timer
- sleep modus
- hardware interrupts
- software interrupts
5. **Interface**: We ontwikkelen een interface voor een OLED-display dat de huidige temperatuur en ventilatorsnelheid weergeeft.

#### **Acceptance criteria**
Het systeem voldoet aan de eisen van de cursushandleiding: 
• watchdog timer
• sleep modus
• hardware interrupts
• software interrupts
• gebruik van RTOS / embedded Linux
• communicatie via I2C en/of SPI
• uitlezen van een sensor
• regelen van een systeem (bij voorkeur met PID-controller)
• aansturen van een actuator (motor driver, relais of iets dergelijks. Een LEDje is geen actuator.)
• interfacing met een display (LCD, OLED of anders; denk aan Nixie-tubes, meerdere LEDs, wijzertje) en knoppen of touchscreen

#### **Beperkingen**
1. **Budget**: We hebben een beperkt budget van 100 euro voor de aanschaf van componenten en materialen.
2. **Tijd**: We hebben een beperkte tijd van 8 weken om het project te voltooien, inclusief ontwerp, implementatie en testen.
3. **Complexiteit**: We moeten ervoor zorgen dat het systeem niet te complex wordt, zodat we binnen de tijdslimiet kunnen blijven en het project haalbaar is voor ons team.
