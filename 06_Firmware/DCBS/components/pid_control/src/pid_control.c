#include "pid_control.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "config_manager.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "state_estimator.h"
#include "esp_timer.h"
#define PID_TASK_STACK_SIZE 3072U
#define PID_TASK_PRIORITY 9U
#define PID_INPUT_TASK_PRIORITY 8U
#define PID_INPUT_QUEUE_LEN 1U
#define PID_OUTPUT_QUEUE_LEN 1U
#define PID_MONITOR_QUEUE_LEN 1U
#define PID_DT_SECONDS 0.01f
#define PID_GAIN_SCALE 1.0f

QueueHandle_t pid_input_queue_handle = NULL;
QueueHandle_t pid_output_queue_handle = NULL;
static QueueHandle_t s_pid_monitor_queue_handle = NULL;
static TaskHandle_t s_pid_task_handle = NULL;
static TaskHandle_t s_pid_input_task_handle = NULL;
static pid_controller_t s_pitch_pid;
static pid_controller_t s_roll_pid;
static float s_pitch_setpoint_deg = 0.0f;
static float s_roll_setpoint_deg = 0.0f;

static void pid_control_apply_settings_from_config(void)
{
    const app_config_t *cfg;

    cfg = config_manager_get();
    if (cfg == NULL)
    {
        pid_init(&s_pitch_pid, 0.0f, 0.0f, 0.0f, -90.0f, 90.0f);
        pid_init(&s_roll_pid, 0.0f, 0.0f, 0.0f, -90.0f, 90.0f);
        s_pitch_setpoint_deg = 0.0f;
        s_roll_setpoint_deg = 0.0f;
        return;
    }

    s_pitch_setpoint_deg = (float)cfg->pid_setpoint;
    s_roll_setpoint_deg = (float)cfg->pid_setpoint;

    pid_init(
        &s_pitch_pid,
        (float)cfg->pid_kp * PID_GAIN_SCALE,
        (float)cfg->pid_ki * PID_GAIN_SCALE,
        (float)cfg->pid_kd * PID_GAIN_SCALE,
        -90.0f,
        90.0f);

    pid_init(
        &s_roll_pid,
        (float)cfg->pid_kp * PID_GAIN_SCALE,
        (float)cfg->pid_ki * PID_GAIN_SCALE,
        (float)cfg->pid_kd * PID_GAIN_SCALE,
        -90.0f,
        90.0f);

    printf("[pid] loaded kp=%.2f ki=%.2f kd=%.2f setpoint=%.2f\n",
           (double)s_pitch_pid.kp,
           (double)s_pitch_pid.ki,
           (double)s_pitch_pid.kd,
           (double)s_pitch_setpoint_deg);
}

static void pid_input_task(void *arg)
{
    QueueHandle_t orientation_queue = NULL;
    (void)arg;

    while (1)
    {
        if (orientation_queue == NULL)
        {
            orientation_queue = state_estimator_get_queue_handle();
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        orientation_t orientation;
        if (xQueueReceive(orientation_queue, &orientation, pdMS_TO_TICKS(100)) == pdPASS)
        {
            pid_input_t in = {
                .pitch_measurement = orientation.pitch,
                .roll_measurement = orientation.roll,
            };

            if (pid_input_queue_handle != NULL)
            {
                xQueueOverwrite(pid_input_queue_handle, &in);
            }
        }
    }
}

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

    // printf("PID update: setpoint=%.2f, measured=%.2f, error=%.2f, p=%.2f, i=%.2f, d=%.2f, output=%.2f\n",
    //        setpoint_deg, measured_angle_deg, error, p_term, pid->ki * pid->integral, d_term, output);
    return output;
}

QueueHandle_t pid_control_get_queue_handle(void)
{
    return pid_output_queue_handle;
}

QueueHandle_t pid_control_get_input_queue_handle(void)
{
    return pid_input_queue_handle;
}

QueueHandle_t pid_control_get_monitor_queue_handle(void)
{
    return s_pid_monitor_queue_handle;
}

esp_err_t pid_control_reload_settings(void)
{
    if (!config_manager_is_initialized())
    {
        return ESP_ERR_INVALID_STATE;
    }

    pid_control_apply_settings_from_config();
    return ESP_OK;
}

esp_err_t pid_control_start(void)
{
    BaseType_t task_created;

    if (s_pid_task_handle != NULL && s_pid_input_task_handle != NULL)
    {
        return ESP_OK;
    }

    if (pid_input_queue_handle == NULL)
    {
        pid_input_queue_handle = xQueueCreate(PID_INPUT_QUEUE_LEN, sizeof(pid_input_t));
        if (pid_input_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    if (pid_output_queue_handle == NULL)
    {
        pid_output_queue_handle = xQueueCreate(PID_OUTPUT_QUEUE_LEN, sizeof(pid_output_t));
        if (pid_output_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    if (s_pid_monitor_queue_handle == NULL)
    {
        s_pid_monitor_queue_handle = xQueueCreate(PID_MONITOR_QUEUE_LEN, sizeof(pid_output_t));
        if (s_pid_monitor_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    pid_control_apply_settings_from_config();

    task_created = xTaskCreate(
        pid_input_task,
        "pid_input_task",
        3072,
        NULL,
        PID_INPUT_TASK_PRIORITY,
        &s_pid_input_task_handle);

    if (task_created != pdPASS)
    {
        s_pid_input_task_handle = NULL;
        return ESP_FAIL;
    }

    task_created = xTaskCreate(
        pid_control_task,
        "pid_control_task",
        PID_TASK_STACK_SIZE,
        NULL,
        PID_TASK_PRIORITY,
        &s_pid_task_handle);

    if (task_created != pdPASS)
    {
        s_pid_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

void pid_control_task(void *arg)
{
    uint32_t log_div = 0U;
    (void)arg;

    while (1)
    {
        if (pid_input_queue_handle == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        uint64_t now_us = esp_timer_get_time();
        static uint64_t last_time_us = 0;

        if (last_time_us == 0)
        {
            last_time_us = now_us;
        }

        float dt = (now_us - last_time_us) / 1000000.0f;
        last_time_us = now_us;

        if (dt <= 0.0f || dt > 0.1f)
        {
            dt = 0.1f;
        }
        
        
        pid_input_t in;
        if (xQueueReceive(pid_input_queue_handle, &in, pdMS_TO_TICKS(100)) == pdPASS)
        {
            pid_output_t out = {
                .pitch_measurement = in.pitch_measurement,
                .roll_measurement = in.roll_measurement,
                .pitch_output = pid_update(&s_pitch_pid, s_pitch_setpoint_deg, in.pitch_measurement, dt),
                .roll_output = pid_update(&s_roll_pid, s_roll_setpoint_deg, in.roll_measurement, dt),
            };

            if (pid_output_queue_handle != NULL)
            {
                xQueueOverwrite(pid_output_queue_handle, &out);
            }
            if (s_pid_monitor_queue_handle != NULL)
            {
                xQueueOverwrite(s_pid_monitor_queue_handle, &out);
            }

            // log_div++;
            // if (log_div >= 20U)
            // {
            //     printf("[pid] sp=%.2f meas=(%.2f, %.2f) out=(%.2f, %.2f)\n",
            //            s_pitch_setpoint_deg,
            //            out.pitch_measurement,
            //            out.roll_measurement,
            //            out.pitch_output,
            //            out.roll_output);
            //     log_div = 0U;
            // }
        }
    }
}