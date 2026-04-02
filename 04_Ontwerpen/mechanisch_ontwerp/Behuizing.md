# DCBS mechnische ontwerp

## overview

The DCBS is a two axis stablization platform designed to maintain a cup in a level position using servo's motors and sensor. the system detects tilt using an MPU6050 sensor and compensates distrubances via a PID control loop.
this document describes the mechanical design of the system, including all 3D printed components and their dimensions.

## system architecture:
the system consists of:
- a fixed base plate 
- a tilting platform 
- a central pivot join
- two servo motors
- two linkaga arms
- a cup holder mounted on top
- an MPU6050 sensor placed at the center of the platform
## mechnical design

### 1. Base Plate

Dimensions: 150*150 *12 mm

Features: two servo cutouts:  size: 42*22 mm 
position: servo 1: X= _45mm,y=0
            servo 2: X= 0, Y=+45mm
central pivor hole: diameter 5mm

### 2. Platform 
DImension: 120*120 * 8 

Features: 
central pivot hole: 5,5 mm 
2* linkage connection holes: diameter: 3,2 mm 

### 3. cup holder
dimensions: outer diameter: 100mm
inner diameter: 85 mm 
height: 25 mm

features:
inner lip: diameter: 83mm; height: 2-3mm
designed to hold standard cups

### 4. linkage arms 2* 

centre to center lenght : 50 mm
holde diameter : 3,2mm 
widht: 10mm
thickness : 4mm 

features: connect servo arms to platfrom 
transfer motion from servo rotation to platfrom tilt



## Sensor placement:

the MPU6050 sensor must be placed:
- at the geometric center of the platform.
- directly above the pivot acis
- firmly mounted to avoid vibration artifacts.
## 3d Printing guigelines.

materials: PETG 
walls: 3 perimeters
infill:

base: 40% 
platform: 30%
cup holder: 25%
link arms: 100% 