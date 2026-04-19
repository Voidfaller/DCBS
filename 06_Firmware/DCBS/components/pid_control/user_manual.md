# PID Controller Component

The **PID controller** outputs control signals to stabilize our system based on measured orientation versus a desired set point. It is intended for our **Dynamic Cup Balancing System (DCBS)** servo motors to control both pitch and roll stabilization.

---

## Features

- Implements a **PID** control loop.
- Supports output limits to prevent actuator saturation.
- Provides functions for initialization, reset, and real time updates.
- Designed for embedded systems with real time control requirements.

---

## How it works

### Initialization :

- Configure PID gains (`kp`, `ki`, `kd`) and output limits using:
  
```c
pid_controller_t pid_pitch;
pid_init(&pid_pitch, 1.0f, 0.5f, 0.1f, -30.0f, 30.0f);

```

### Update control output

- Call pid_update() each control cycle with the desired set point and measured angle.
- Computes the error, applies p, i and d terms, and clamps the output.

```c
float servo_command = pid_update(&pid_pitch, target_pitch, measured_pitch, dt);
```
### Reset integral term:

- Use pid_reset() to clear the integral and previous measurement if needed.
  
```c 
pid_reset(&pid_pitch);
```
---

## PID Algorithm:

- p (proportional) : provides immediate response to error. 
$p = kp * error$
- i (integral): accumulates over time to eliminate steady-state error.
$i += ki * error * dt$
- d (derivative): based on the negative slope of the measured angle for damping. 
$d = -kd * rate~of~change$

---

## API reference

| Function                                                                                           | Description                                                                                                                 |
| -------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| `void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float out_min, float out_max)` | Initializes the PID controller with the specified gains (`Kp`, `Ki`, `Kd`) and output limits (`out_min`, `out_max`).        |
| `void pid_reset(pid_controller_t *pid)`                                                            | Resets the integral term and previous measurement of the PID controller.                                                    |
| `float pid_update(pid_controller_t *pid, float setpoint_deg, float measured_angle_deg, float dt)`  | Computes the PID output for a given setpoint and measured angle over a timestep `dt` (seconds). Returns the control output. |
