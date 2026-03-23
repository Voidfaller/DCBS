# PID Controller Component

The **PID Controller** is a module for computing control outputs to stabilize a system based on measured orientation versus a desired setpoint. It is intended for use in the **Dynamic Cup Balancing System (DCBS)** to control servo motors for pitch and roll stabilization.

---

## Features

- Implements a **PID** control loop.
- Supports output limits to prevent actuator saturation.
- Provides functions for initialization, reset, and real time updates.
- Designed for embedded systems with real time control requirements.

---

## How It Works

### Initialization :

- Configure PID gains (`Kp`, `Ki`, `Kd`) and output limits.
  
```c
pid_controller_t pid_pitch;
pid_init(&pid_pitch, 1.0f, 0.5f, 0.1f, -30.0f, 30.0f);

```

### update control output

- call pid_update() each control cyvle with the desired setpoint and measured angle.
- computes the error, applies P,I and D terms, and clamps the output

```c
float servo_command = pid_update(&pid_pitch, target_pitch, measured_pitch, dt);
```
### reset intergral term:

- use pid_reset() to clear the integral and previous measurement if needed.
  
```c 
pid_reset(&pid_pitch);
```

## PID algorithm:

- P (proportional) : Provides immediate response to error. P = Kp * error
-  I (integral): Accumulates over time to eliminate steady-state error. I += Ki * error * dt
-  D (Derivavite): Based on the negative slope of the measured angle for damping. D = -Kd * rate_of_change

## API reference 

## API Reference

| Function | Description |
|----------|-------------|
| `void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float out_min, float out_max)` | Initializes the PID controller with the specified gains (`Kp`, `Ki`, `Kd`) and output limits (`out_min`, `out_max`). |
| `void pid_reset(pid_controller_t *pid)` | Resets the integral term and previous measurement of the PID controller. |
| `float pid_update(pid_controller_t *pid, float setpoint_deg, float measured_angle_deg, float dt)` | Computes the PID output for a given setpoint and measured angle over a timestep `dt` (seconds). Returns the control output. |