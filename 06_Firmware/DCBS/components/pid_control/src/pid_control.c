#include "pid_control.h"
#include <stddef.h>

void pid_init(pid_controller_t *pid,
              float kp,
              float ki,
              float kd,
              float out_min,
              float out_max)  
{
  if (pid == NULL)
      return;

  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->integral = 0.0f;
  pid->prev_measurement = 0.0f;
  pid->out_min = out_min;
  pid->out_max = out_max;
}

void pid_reset(pid_controller_t *pid) 
{
  if (pid == NULL)
      return;
    
  pid->integral = 0.0f;
  pid->prev_measurement = 0.0f;
}

float pid_update(pid_controller_t *pid,
                float setpoint_deg,
                float measured_angle_deg,
                float dt)
{
    if (pid == NULL || dt <= 0.0f)
        return 0.0f;

    float error = setpoint_deg - measured_angle_deg;

    float p_term = pid->kp * error;

    float old_integral = pid->integral;
    pid->integral += error * dt;

    float measurement_rate = (measured_angle_deg - pid->prev_measurement) / dt;
    float d_term = -pid->kd * measurement_rate;

    float output = p_term + (pid->ki * pid->integral) + d_term;

    if (output > pid->out_max)
    {
        output = pid->out_max;
        pid->integral = old_integral;
    }
    else if (output < pid->out_min)
    {
        output = pid->out_min;
        pid->integral = old_integral;
    }

    pid->prev_measurement = measured_angle_deg;

    return output;
}