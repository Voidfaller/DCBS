#include "servo_driver.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define SERVO_MIN_ANGLE_DEG (-90.0f)
#define SERVO_MAX_ANGLE_DEG (90.0f)
#define SERVO_TASK_PERIOD_MS (20)
#define SERVO_TASK_STEP_DEG (2.0f)
#define SERVO_CMD_QUEUE_LEN (8)

static QueueHandle_t s_servo_cmd_queue = NULL;
static TaskHandle_t s_servo_task_handle = NULL;

static float clamp_angle(float angle)
{
    if (angle < SERVO_MIN_ANGLE_DEG)
    {
        return SERVO_MIN_ANGLE_DEG;
    }
    if (angle > SERVO_MAX_ANGLE_DEG)
    {
        return SERVO_MAX_ANGLE_DEG;
    }
    return angle;
}

static void servo_task(void *arg)
{
    servo_t *servo = (servo_t *)arg;
    servo_cmd_t cmd;
    float current_angle = clamp_angle(servo->current_angle);
    float target_angle = clamp_angle(servo->target_angle);

    while (1)
    {
        while (xQueueReceive(s_servo_cmd_queue, &cmd, 0) == pdPASS)
        {
            target_angle = clamp_angle(cmd.target_angle);
            if (cmd.immediate)
            {
                current_angle = target_angle;
            }
        }

        if (current_angle < (target_angle - SERVO_TASK_STEP_DEG))
        {
            current_angle += SERVO_TASK_STEP_DEG;
        }
        else if (current_angle > (target_angle + SERVO_TASK_STEP_DEG))
        {
            current_angle -= SERVO_TASK_STEP_DEG;
        }
        else
        {
            current_angle = target_angle;
        }

        servo->current_angle = current_angle;
        servo->target_angle = target_angle;
        servo_rotate(servo, current_angle);
        vTaskDelay(pdMS_TO_TICKS(SERVO_TASK_PERIOD_MS));
    }
}

// Initialize the servo
void servo_init(servo_t *servo)
{
    servo->current_angle = clamp_angle(servo->current_angle);
    servo->target_angle = clamp_angle(servo->target_angle);

    // Configure the LEDC timer
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50, // Standard servo frequency
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer_conf);

    // Configure the LEDC channel for the servo
    ledc_channel_config_t ch_conf = {
        .gpio_num = servo->gpio_pin,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ch_conf);

    int pulse = angle_to_pulse_us(servo, servo->current_angle);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pulse);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

// Rotate the servo to a specific angle
void servo_rotate(servo_t *servo, float angle)
{
    servo->target_angle = angle;
    int pulse = angle_to_pulse_us(servo, angle);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pulse);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

int angle_to_pulse_us(servo_t *servo, float angle)
{
    angle = clamp_angle(angle);

    // For centered servos: -90° to +90° maps to min_pulse_us to max_pulse_us
    // Convert angle range from [-90, +90] to [0, 180] then calculate pulse
    float pulse_us = servo->min_pulse_us + ((angle + 90.0) / 180.0) * (servo->max_pulse_us - servo->min_pulse_us);
    
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

    if (s_servo_task_handle != NULL)
    {
        return ESP_OK;
    }

    s_servo_cmd_queue = xQueueCreate(SERVO_CMD_QUEUE_LEN, sizeof(servo_cmd_t));
    if (s_servo_cmd_queue == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    BaseType_t task_created = xTaskCreate(
        servo_task,
        "servo_task",
        2048,
        servo,
        5,
        &s_servo_task_handle);

    if (task_created != pdPASS)
    {
        vQueueDelete(s_servo_cmd_queue);
        s_servo_cmd_queue = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t servo_send_command(servo_t *servo, const servo_cmd_t *cmd)
{
    (void)servo;

    if (cmd == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_servo_cmd_queue == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (xQueueSend(s_servo_cmd_queue, cmd, pdMS_TO_TICKS(20)) != pdPASS)
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

