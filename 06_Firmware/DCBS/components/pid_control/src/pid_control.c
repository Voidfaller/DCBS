#include "pid_control.h"

#include "esp_timer.h"
#include "freertos/task.h"
#include "watchdog_service.h"
#include "state_estimator.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define PID_ANGLE_NORM_DEG 90.0f
#define PID_ERROR_DEADBAND 0.01f
#define PID_INTEGRAL_LIMIT 1.0f
#define PID_OUTPUT_LIMIT 1.0f
#define PID_DERIVATIVE_ALPHA 0.85f
#define PID_TERMINAL_BUFFER_SIZE 96U
static QueueHandle_t s_pid_queue_handle = NULL;
QueueHandle_t s_state_estimator_queue_handle = NULL; // Expose state estimator queue handle for testing
// forware declarations
static void pid_control_task(void *arg);
static void pid_terminal_task(void *arg);
// PID controllers for pitch and roll
// PID controller for pitch axis
pid_controller_t s_pitch_pid = {
    .kp = 0.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .setpoint = 0.0f,
};
// PID controller for roll axis
pid_controller_t s_roll_pid = {
    .kp = 0.0f,
    .ki = 0.0f,
    .kd = 0.0f,
    .setpoint = 0.0f,
};
// helper functions for terminal interface to print settings and process commands
static void pid_print_settings(void)
{
    printf("[PID Terminal] Pitch: setpoint=%.2f kp=%.3f ki=%.3f kd=%.3f\n", 
           s_pitch_pid.setpoint,
           s_pitch_pid.kp,
           s_pitch_pid.ki,
           s_pitch_pid.kd); 
    printf("[PID Terminal] Roll : setpoint=%.2f kp=%.3f ki=%.3f kd=%.3f\n",
           s_roll_pid.setpoint,
           s_roll_pid.kp,
           s_roll_pid.ki,
           s_roll_pid.kd);
}
// update a specific PID parameter based on terminal command input
static bool pid_set_term_value(pid_controller_t *pid, const char *term, float value) 
{
    if ((pid == NULL) || (term == NULL))
    {
        return false;
    }

    if (strcmp(term, "kp") == 0)
    {
        pid->kp = value;
    }
    else if (strcmp(term, "ki") == 0)
    {
        pid->ki = value;
    }
    else if (strcmp(term, "kd") == 0)
    {
        pid->kd = value;
    }
    else if (strcmp(term, "setpoint") == 0)
    {
        pid->setpoint = value;
    }
    else
    {
        return false;
    }

    return true;
}
// parse and process a terminal command line 
static void pid_process_terminal_line(const char *line)
{
    char axis[8];
    char term[16];
    float value;
    int parsed;

    if (line == NULL)
    {
        return;
    }
    // help command
    if ((strcmp(line, "pid help") == 0) || (strcmp(line, "help pid") == 0))
    {
        printf("[PID Terminal] Commands:\n");
        printf("[PID Terminal]   pid show\n");
        printf("[PID Terminal]   pid <pitch|roll|both> <kp|ki|kd|setpoint> <value>\n");
        printf("[PID Terminal]   recal\n");
        printf("[PID Terminal] Examples:\n");
        printf("[PID Terminal]   pid roll kp 3.2\n");
        printf("[PID Terminal]   pid both kd 0.8\n");
        printf("[PID Terminal]   pid both setpoint 0\n");
        printf("[PID Terminal]   recal\n");
        return;
    }
    // show current PID settings
    if (strcmp(line, "pid show") == 0)
    {
        pid_print_settings();
        return;
    }
    // recalibrate IMU
    if (strcmp(line, "recal") == 0)
    {
        printf("[Terminal] Starting IMU recalibration. Keep the board level and still for 1 second...\n");
        state_estimator_recalibrate();
        return;
    }
    // parse command like: pid roll kp 3.6
    parsed = sscanf(line, "pid %7s %15s %f", axis, term, &value);
    if (parsed == 3)
    {
        bool ok = true;
        if (strcmp(axis, "pitch") == 0)
        {
            ok = pid_set_term_value(&s_pitch_pid, term, value);
        }
        else if (strcmp(axis, "roll") == 0)
        {
            ok = pid_set_term_value(&s_roll_pid, term, value);
        }
        else if (strcmp(axis, "both") == 0)
        {
            ok = pid_set_term_value(&s_pitch_pid, term, value) &&
                 pid_set_term_value(&s_roll_pid, term, value);
        }
        else
        {
            ok = false;
        }

        if (ok)
        {
            printf("[PID Terminal] Updated %s %s to %.3f\n", axis, term, value);
            pid_print_settings();
        }
        else
        {
            printf("[PID Terminal] Invalid command. Type: pid help\n");
        }
        return;
    }
    // unkown PID command
    if (strncmp(line, "pid", 3) == 0)
    {
        printf("[PID Terminal] Invalid command. Type: pid help\n");
    }
}
// task that listens for terminal input and updates PID parameters
static void pid_terminal_task(void *arg)
{
    (void)arg;
    char line[PID_TERMINAL_BUFFER_SIZE];

    while (1)
    {
        // read line from terminal input stdin
        if (fgets(line, sizeof(line), stdin) != NULL)
        {
            size_t len = strlen(line);
            // remove newline characters
            while ((len > 0U) && ((line[len - 1U] == '\n') || (line[len - 1U] == '\r')))
            {
                line[len - 1U] = '\0';
                len--;
            }

            if (len > 0U)
            {
                pid_process_terminal_line(line);
            }
        }
        else
        {
            // if no input, wait a bit
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}
// initialize PID controllers (reset state variables)
void pid_controller_init()
{
    s_pitch_pid.integral = 0.0f;
    s_pitch_pid.last_error = 0.0f;
    s_pitch_pid.filtered_derivative = 0.0f;
    s_pitch_pid.last_time_us = esp_timer_get_time();

    s_roll_pid.integral = 0.0f;
    s_roll_pid.last_error = 0.0f;
    s_roll_pid.filtered_derivative = 0.0f;
    s_roll_pid.last_time_us = esp_timer_get_time();
}
// update PID controller based on current measurement
float pid_controller_update(pid_controller_t *pid, float measurement)
{
    uint64_t now_us;
    float dt;
    float setpoint;
    float error;
    float raw_derivative;
    float output;

    if (pid == NULL)
    {
        return 0.0f;
    }
    // Normalize measurement and setpoint to -1,1
    measurement /= PID_ANGLE_NORM_DEG;
    setpoint = pid->setpoint / PID_ANGLE_NORM_DEG;
    // compute time step dt
    now_us = esp_timer_get_time();
    dt = (now_us - pid->last_time_us) / 1000000.0f;
    if (dt <= 0.0f)
    {
        dt = 0.001f; // prevent division by zero
    }
    pid->last_time_us = now_us;
    // compute error
    error = setpoint - measurement;
    if (fabsf(error) < PID_ERROR_DEADBAND) // apply deadband to reduce noise around zero
    {
        error = 0.0f;
    }

    pid->integral += error * dt; // intergral term with ant windup clamp
    if (pid->integral > PID_INTEGRAL_LIMIT)
    {
        pid->integral = PID_INTEGRAL_LIMIT;
    }
    else if (pid->integral < -PID_INTEGRAL_LIMIT)
    {
        pid->integral = -PID_INTEGRAL_LIMIT;
    }
    // derivative term (rate of change of error)
    raw_derivative = (error - pid->last_error) / dt;
    pid->filtered_derivative = // apply low filter to derivative (reduce noise)
        (PID_DERIVATIVE_ALPHA * pid->filtered_derivative) +
        ((1.0f - PID_DERIVATIVE_ALPHA) * raw_derivative);
    pid->last_error = error;
    // compute total PID output and clamp to limits 
    output = (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * pid->filtered_derivative);
    if (output > PID_OUTPUT_LIMIT)
    {
        output = PID_OUTPUT_LIMIT;
    }
    else if (output < -PID_OUTPUT_LIMIT)
    {
        output = -PID_OUTPUT_LIMIT;
    }

    return output;
}
// return queue handle for PID output
QueueHandle_t pid_control_get_queue_handle(void)
{
    return s_pid_queue_handle;
}
// start PID control and terminal tasks
esp_err_t pid_control_start(void)
{
    BaseType_t task_ok;
    BaseType_t terminal_task_ok;
    // create queue for PID output
    if (s_pid_queue_handle == NULL)
    {
        s_pid_queue_handle = xQueueCreate(1, sizeof(pid_correction_t));
        if (s_pid_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }
    // get queue from state estimator 
    if (s_state_estimator_queue_handle == NULL)
    {
        s_state_estimator_queue_handle = state_estimator_get_queue_handle();
        if (s_state_estimator_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }
    // start PID control task 
    task_ok = xTaskCreate(pid_control_task, "PID Control Task", 2048, NULL, 10, NULL);
    if (task_ok != pdPASS)
    {
        return ESP_FAIL;
    }
    // start terminal task for PID tuning
    terminal_task_ok = xTaskCreate(pid_terminal_task, "PID Terminal Task", 3072, NULL, 5, NULL);
    if (terminal_task_ok != pdPASS)
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}
// Main PID control loop task: waits for orientation updates, computes PID corrections, and sends them to 
// motor control task via queue
static void pid_control_task(void *arg)
{
    (void)arg;
    uint32_t log_divider = 0U;

    pid_controller_init();
    printf("[PID Terminal] Ready. Type 'pid help' for commands.\n");
    pid_print_settings();

    pid_correction_t correction;

    orientation_t current_orientation;
    while (1)
    {
        // wait for new orientation data from state estimator 
        if (xQueueReceive(s_state_estimator_queue_handle, &current_orientation, pdMS_TO_TICKS(100)) == pdPASS)
        {
            // compute PID outputs for pitch and roll based on current orientation 
            correction.pitch_pwm = pid_controller_update(&s_pitch_pid, current_orientation.pitch);
            correction.roll_pwm = pid_controller_update(&s_roll_pid, current_orientation.roll);
            xQueueOverwrite(s_pid_queue_handle, &correction); // send latest correction to servo task

            // log_divider++;
            // if (log_divider >= 20U)
            // {
            //     printf("[PID Control] Received orientation: pitch:%.2f roll:%.2f\n", current_orientation.pitch, current_orientation.roll);
            //     printf("[PID Control] Computed corrections: pitch:%.3f roll:%.3f\n", correction.pitch_pwm, correction.roll_pwm);
            //     log_divider = 0U;
            // }
        }
    }
}