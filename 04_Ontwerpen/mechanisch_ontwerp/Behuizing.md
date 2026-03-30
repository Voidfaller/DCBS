# Mechanisch ontwerp

## 1. Onderdelen

Onderdelen:
Base (onderplaat)
Servo X-as
Servo Y-as
Inner frame (kantelplatform)
Cup holder
MPU6050 sensor
ESP32 + voeding
Display

De servo’s staan haaks op elkaar zodat ze twee richtingen kunnen corrigeren.

Beweging:
Servo 1 : kantelt links/rechts (X-axis)
Servo 2 : kantelt voor/achter (Y-axis)

## Belangrijke Basismaten:

gebaseerd op de requirements+onderdelen:

**Beker** : Diameter: 80mm ; Platform: 100 mm 
**Servo** : Body: ~40*20 *40 mm; Torque: voldoende voor 1kg.

## 2. Afmetingen

### 2.1 BasePlat:

150*150 *8 mm
servo mount gaten en mcu gaten esp32 en rubber feet

Rubber feet:
4 gaten:
diameter 6 mm
diepte 3 mm
hoekposities:
10 mm van elke rand

Servo MG996R mounting holes:
2 gaten 
Diameter = 3 mm
afstand 49 
Termination = Through All

### 2.2 Outframe X-as

120*120 * 
binnen Frame: 12 


### 2.3 InnerFrame Y-as

100 *100 *
Binnen Frame: 10mm 

### 2.4 Cup holder 
Cirkel: 100
binnen cirkel 85 
Extrude 3mm 

### 2.5 Servo bracket
60*40 * 

twee gaten 
15mm / boven


## 3. Hardware 


Base Plate: Voeding - Esp32 - Servo 
Servo Bracket:  Houdt servo X
Outer Frame: draagt inner frame, draait x-as 
inner frame: Draagt Cup Holder, tilt sensor 


-------------------------

Nieuwe versie:

1. Base Plate
afmetingen: 200* 200 mm
dikte: 6mm

voeg toe: 
servo gaten:
- 2 servo positites links/rechts
- hartafstand: 120mm

gat maten:
- M3 gaten : 3,2mm


Elektronica ruimte:
- uistparing: 80*60 mm
- diepte: 3mm 
voor ESP32 en MPU6050

2. Servo Mount: dit kan je ook online vinden

maak een L-beugel:
base: 60*40mm 
Hoogte: 50mm
dikte:4mm

3. Outer Frame:
vierkant ring
buitenmaat: 140*140 mm
binnenmaat: 110*110 mm
dikte 6 mm

-servo connectie: aan beide zijkanten: gat voor as: 6mm dit is waar de servo horn komt


4. Inner frame:
buitenmaat: 110*110 mm
Binnenmaat: 90*90 mm
dikte: 6mm
zijkanen: 6mm gaten voor rotatie ; 90 gedraaid t.o.v outerframe


5. platform (cup holder)

diamter: 100 mm
dikte: 5mm

rand: hoogte: 10mm ; dikte: 3mm; voorkomt fat beker schuift 
sensor mount: klein vlak: 20*20mm voor mpu6050