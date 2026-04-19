# Problem Description

## Project Context

For the course TINLAB – Embedded Systems, a system will be designed and developed to assist individuals with muscle disorders in drinking independently. The goal is a self-balancing cup holder that maintains stability despite involuntary muscle spasms, improving accessibility and daily comfort.

## Scientific Background

Recent research highlights both the challenge of involuntary movement and emerging assistive approaches that justify the need for adaptive stabilization technology:

**Functional disability from tremor and involuntary motion is a significant barrier to daily activities.** Tremors and uncontrolled muscle activity impair tasks such as eating, drinking, and writing, leaving many individuals with movement disorders unable to perform these activities independently. Assistive devices capable of mitigating these limitations could significantly improve quality of life.

**Mechanical and active stabilization strategies can dramatically reduce involuntary motion during object manipulation.** Research on robotic eating devices such as a tremor‑stabilizing spoon using IMU sensors and active control has shown vibration suppression between ~84 % and ~99 % and maintained stability of the utensil tip despite tremors.

**Emerging wearable technologies demonstrate the feasibility of compensating involuntary movement using sensors and adaptive control.** Wearable assistive systems that fuse EMG and inertial sensors can decrease tremor prominence and improve movement quality in activities of daily living, indicating that real‑time sensor feedback and control can effectively counteract motor instability.

**Innovative technologies such as artificial muscles or exoskeletons are being developed to suppress tremors and support fine motor tasks.** For instance, research on biorobotic arms with HASEL artificial muscles that dynamically counteract wrist and hand tremors suggests future wearable assistive systems could help individuals maintain stable object handling like holding a cup.

Together, these findings show that involuntary motions caused by neuromuscular instability can be mitigated using sensor‑driven, adaptive systems, and that embedded technologies present a viable path toward assistive devices supporting safe, independent daily activities such as your proposed self‑balancing cup holder.

## Problem Statement
While assistive devices exist to support mobility and gross motor functions, there is a lack of practical, adaptive solutions for small, precise daily tasks such as drinking. Current approaches are:
- Manual r static (requiring constant human adjustment)
- Not responsive to involuntary muscle activity
- Expensive or complex to implement in everyday settings
 
There is a need for an embedded system that:
- Detects tilt, motion, or perturbations of a cup - using sensors (e.g., gyroscope, accelerometer)
- Automatically stabilizes the cup to prevent spills
- Adapts to different levels of muscle spasm intensity and speed
- Is safe, compact, and usable in multiple environments (home, office, classroom, recreational spaces)

## Assignment Description
Design and implement a Dynamic Cup Balancing System (DCBS) that automatically compensates for involuntary movements, stabilizes a cup in real time, and enables individuals with muscle disorders to drink safely and independently.

## Target Users:
- Individuals with muscular disorders causing spasms or tremors
- People with temporary motor impairments (e.g., post-injury or fatigue)
- Caregivers looking for safer assistive solutions
- Anyone who wants to reduce the risk of spills while drinking