#include "servo_driver.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define SERVO_DEFAULT_MIN_ANGLE_DEG (-90.0f)
#define SERVO_DEFAULT_MAX_ANGLE_DEG (90.0f)
#define SERVO_DEFAULT_TASK_PERIOD_MS (20U)
#define SERVO_DEFAULT_TASK_STEP_DEG (2.0f)
#define SERVO_DEFAULT_CMD_QUEUE_LEN (8U)
#define SERVO_DEFAULT_TASK_STACK_SIZE (2048U)
#define SERVO_DEFAULT_TASK_PRIORITY (5U)

static const char *TAG = "servo"; //Logging tag

void servo_driver_init(void)
{
    ESP_LOGI(TAG, "Servo driver is initialized!!"); // Log test
}

static void servo_apply_defaults(servo_t *servo)
{
    if (servo->min_angle_deg >= servo->max_angle_deg)
    {
        servo->min_angle_deg = SERVO_DEFAULT_MIN_ANGLE_DEG;
        servo->max_angle_deg = SERVO_DEFAULT_MAX_ANGLE_DEG;
    }

    if (servo->task_period_ms == 0U)
    {
        servo->task_period_ms = SERVO_DEFAULT_TASK_PERIOD_MS;
    }

    if (servo->task_step_deg <= 0.0f)
    {
        servo->task_step_deg = SERVO_DEFAULT_TASK_STEP_DEG;
    }

    if (servo->cmd_queue_len == 0U)
    {
        servo->cmd_queue_len = SERVO_DEFAULT_CMD_QUEUE_LEN;
    }

    if (servo->task_stack_size == 0U)
    {
        servo->task_stack_size = SERVO_DEFAULT_TASK_STACK_SIZE;
    }

    if (servo->task_priority == 0U)
    {
        servo->task_priority = SERVO_DEFAULT_TASK_PRIORITY;
    }

    if (servo->speed_mode != LEDC_HIGH_SPEED_MODE && servo->speed_mode != LEDC_LOW_SPEED_MODE)
    {
        servo->speed_mode = LEDC_HIGH_SPEED_MODE;
    }

    if (servo->timer_num > LEDC_TIMER_3)
    {
        servo->timer_num = LEDC_TIMER_0;
    }

    if (servo->channel > LEDC_CHANNEL_7)
    {
        servo->channel = LEDC_CHANNEL_0;
    }
}

// Clamp angle to servo limits
static float clamp_angle(const servo_t *servo, float angle)
{
    if (angle < servo->min_angle_deg)
    {
        return servo->min_angle_deg;
    }
    if (angle > servo->max_angle_deg)
    {
        return servo->max_angle_deg;
    }
    return angle;
}

// Servo control task
static void servo_task(void *arg)
{
    servo_t *servo = (servo_t *)arg;
    QueueHandle_t cmd_queue = (QueueHandle_t)servo->cmd_queue_handle;
    servo_cmd_t cmd;
    float current_angle = clamp_angle(servo, servo->current_angle);
    float target_angle = clamp_angle(servo, servo->target_angle);

    while (1)
    {
        while (xQueueReceive(cmd_queue, &cmd, 0) == pdPASS)
        {
            target_angle = clamp_angle(servo, cmd.target_angle);
            if (cmd.immediate)
            {
                current_angle = target_angle;
            }
        }

        if (current_angle < (target_angle - servo->task_step_deg))
        {
            current_angle += servo->task_step_deg;
        }
        else if (current_angle > (target_angle + servo->task_step_deg))
        {
            current_angle -= servo->task_step_deg;
        }
        else
        {
            current_angle = target_angle;
        }

        servo->current_angle = current_angle;
        servo->target_angle = target_angle;
        servo_rotate(servo, current_angle);
        vTaskDelay(pdMS_TO_TICKS(servo->task_period_ms));
    }
}

// Initialize the servo
void servo_init(servo_t *servo)
{
    servo_apply_defaults(servo);
    servo->current_angle = clamp_angle(servo, servo->current_angle);
    servo->target_angle = clamp_angle(servo, servo->target_angle);

    // Configure the LEDC timer
    ledc_timer_config_t timer_conf = {
        .speed_mode = servo->speed_mode,
        .timer_num = servo->timer_num,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50, // Standard servo frequency
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer_conf);

    // Configure the LEDC channel for the servo
    ledc_channel_config_t ch_conf = {
        .gpio_num = servo->gpio_pin,
        .speed_mode = servo->speed_mode,
        .channel = servo->channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = servo->timer_num,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ch_conf);

    int pulse = angle_to_pulse_us(servo, servo->current_angle);
    ledc_set_duty(servo->speed_mode, servo->channel, pulse);
    ledc_update_duty(servo->speed_mode, servo->channel);
}

// Rotate the servo to a specific angle
void servo_rotate(servo_t *servo, float angle)
{
    servo->target_angle = clamp_angle(servo, angle);
    int pulse = angle_to_pulse_us(servo, servo->target_angle);
    ledc_set_duty(servo->speed_mode, servo->channel, pulse);
    ledc_update_duty(servo->speed_mode, servo->channel);
}

int angle_to_pulse_us(servo_t *servo, float angle)
{
    angle = clamp_angle(servo, angle);

    // Map configured angle range [min_angle_deg, max_angle_deg] to pulse range.
    float angle_span = servo->max_angle_deg - servo->min_angle_deg;
    float normalized = (angle - servo->min_angle_deg) / angle_span;
    float pulse_us = servo->min_pulse_us + (normalized * (servo->max_pulse_us - servo->min_pulse_us));
    
    // Convert to LEDC duty cycle (16-bit timer at 50Hz = 20ms period)
    // duty = (pulse_us / 20000) * 65536
    int duty = (int)((pulse_us / 20000.0) * 65536.0);
    
    return duty;
}

esp_err_t servo_start_task(servo_t *servo)
{
    if (servo == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (servo->task_handle != NULL)
    {
        return ESP_OK;
    }

    servo_apply_defaults(servo);

    QueueHandle_t cmd_queue = xQueueCreate(servo->cmd_queue_len, sizeof(servo_cmd_t));
    if (cmd_queue == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    servo->cmd_queue_handle = (void *)cmd_queue;
    TaskHandle_t task_handle = NULL;

    BaseType_t task_created = xTaskCreate(
        servo_task,
        "servo_task",
        servo->task_stack_size,
        servo,
        servo->task_priority,
        &task_handle);

    if (task_created != pdPASS)
    {
        vQueueDelete(cmd_queue);
        servo->cmd_queue_handle = NULL;
        servo->task_handle = NULL;
        return ESP_FAIL;
    }

    servo->task_handle = (void *)task_handle;

    return ESP_OK;
}

esp_err_t servo_send_command(servo_t *servo, const servo_cmd_t *cmd)
{
    if (servo == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (cmd == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    QueueHandle_t cmd_queue = (QueueHandle_t)servo->cmd_queue_handle;
    if (cmd_queue == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (xQueueSend(cmd_queue, cmd, pdMS_TO_TICKS(20)) != pdPASS)
    {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

esp_err_t servo_set_target(servo_t *servo, float angle, bool immediate)
{
    servo_cmd_t cmd = {
        .target_angle = angle,
        .immediate = immediate};

    return servo_send_command(servo, &cmd);
}

