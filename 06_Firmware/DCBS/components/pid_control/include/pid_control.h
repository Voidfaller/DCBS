#ifndef PID_CONTROL_H
#define PID_CONTROL_H

typedef struct {
    float kp;
    float ki;
    float kd;

    float integral;
    float prev_measurement;

    float out_min;
    float out_max;
} pid_controller_t;

void pid_init(pid_controller_t *pid,
                float kp,
                float ki,
                float kd,
                float out_min,
                float out_max);

void pid_reset(pid_controller_t *pid);

float pid_update (pid_controller_t *pid,
                 float setpoint_deg,
                 float measured_angle_deg,
                 float dt);

#endif /* PID_CONTROL_H */