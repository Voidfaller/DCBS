#include "pid_control.h"

#include "esp_timer.h"
#include "freertos/task.h"
#include "watchdog_service.h"
static QueueHandle_t s_pid_queue_handle = NULL;
static void pid_control_task(void *arg);
void pid_controller_init(pid_controller_t *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->last_time_us = esp_timer_get_time();
}

float pid_controller_update(pid_controller_t *pid, orientation_t measurement)
{
    uint64_t now_us;
    float dt;
    float error;
    float derivative;
    float output;

    if (pid == NULL)
    {
        return 0.0f;
    }

    now_us = esp_timer_get_time();
    dt = (now_us - pid->last_time_us) / 1000000.0f;
    if (dt <= 0.0f)
    {
        dt = 0.001f;
    }
    pid->last_time_us = now_us;

    error = pid->setpoint - measurement.pitch;
    pid->integral += error * dt;
    derivative = (error - pid->last_error) / dt;
    pid->last_error = error;

    output = (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative);
    return output;
}

QueueHandle_t pid_control_get_queue_handle(void)
{
    return s_pid_queue_handle;
}

esp_err_t pid_control_start(void)
{
    BaseType_t task_ok;

    if (s_pid_queue_handle == NULL)
    {
        s_pid_queue_handle = xQueueCreate(1, sizeof(orientation_t));
        if (s_pid_queue_handle == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    task_ok = xTaskCreate(pid_control_task, "PID Control Task", 2048, NULL, 10, NULL);
    if (task_ok != pdPASS)
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void pid_control_task(void *arg)
{
    (void)arg;
    pid_controller_t pid;
    pid_controller_init(&pid);

    while (1)
    {
        printf("[PID Control] Task running...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}