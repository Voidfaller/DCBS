#include "servo_driver.h"
#include "pid_control.h"
#include "config_manager.h"
#include "state_estimator.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <math.h>
#include <stdio.h>

#define SERVO_DEFAULT_MIN_ANGLE_DEG (-90.0f)
#define SERVO_DEFAULT_MAX_ANGLE_DEG (90.0f)
#define SERVO_DEFAULT_TASK_PERIOD_MS (20U)
#define SERVO_DEFAULT_TASK_STEP_DEG (6.0f)
#define SERVO_DEFAULT_CMD_QUEUE_LEN (8U)
#define SERVO_DEFAULT_TASK_STACK_SIZE (2048U)
#define SERVO_DEFAULT_TASK_PRIORITY (5U)
#define SERVO_PERF_LOG_INTERVAL_US (2000000ULL)

#define PITCH_SERVO_GPIO 18
#define PITCH_SERVO_NEUTRAL_DEG 0.0f
#define PITCH_SERVO_GAIN 1.0f
#define PITCH_SERVO_DIRECT_GAIN 1.0f
#define SERVO_PID_OUTPUT_DEADBAND 0.2f
#define SERVO_DIRECT_ANGLE_DEADBAND_DEG 0.5f
#define SERVO_MIN_PULSE_US_LIMIT 500.0f
#define SERVO_MAX_PULSE_US_LIMIT 2500.0f
#define SERVO_MIN_EFFECTIVE_SPAN_US 800.0f

static TaskHandle_t s_pitch_servo_bridge_task = NULL;
static volatile bool s_pitch_servo_pid_bypass = false;
static servo_t s_pitch_servo = {
    .gpio_pin = PITCH_SERVO_GPIO,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_num = LEDC_TIMER_1,
    .channel = LEDC_CHANNEL_0,
    .current_angle = PITCH_SERVO_NEUTRAL_DEG,
    .target_angle = PITCH_SERVO_NEUTRAL_DEG,
    .min_angle_deg = -90.0f,
    .max_angle_deg = 90.0f,
    .min_pulse_us = 1000.0f,
    .max_pulse_us = 2000.0f,
    .task_period_ms = 20,
    .task_step_deg = 6.0f,
    .cmd_queue_len = 1,
    .task_stack_size = 2048,
    .task_priority = 6,
};

static void pitch_servo_stabilizer_task(void *arg)
{
    QueueHandle_t pid_queue = NULL;
    QueueHandle_t orientation_queue = NULL;
    QueueHandle_t servo_cmd_queue = NULL;
    (void)arg;
    uint32_t trace_div = 0U;

    while (1)
    {
        if (servo_cmd_queue == NULL)
        {
            servo_cmd_queue = (QueueHandle_t)s_pitch_servo.cmd_queue_handle;
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (!s_pitch_servo_pid_bypass && pid_queue == NULL)
        {
            pid_queue = pid_control_get_queue_handle();
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (s_pitch_servo_pid_bypass && orientation_queue == NULL)
        {
            orientation_queue = state_estimator_get_queue_handle();
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (s_pitch_servo_pid_bypass)
        {
            orientation_t orientation;
            if (xQueueReceive(orientation_queue, &orientation, pdMS_TO_TICKS(100)) == pdPASS)
            {
                float pitch = orientation.pitch;
                if (fabsf(pitch) < SERVO_DIRECT_ANGLE_DEADBAND_DEG)
                {
                    pitch = 0.0f;
                }

                float target = PITCH_SERVO_NEUTRAL_DEG - (pitch * PITCH_SERVO_DIRECT_GAIN);
                servo_cmd_t cmd = {
                    .target_angle = target,
                    .immediate = false,
                };
                xQueueOverwrite(servo_cmd_queue, &cmd);

                trace_div++;
                if (trace_div >= 20U)
                {
                    printf("[servo] direct pitch=%.2f target=%.2f\n", pitch, target);
                    trace_div = 0U;
                }
            }
            continue;
        }

        pid_output_t pid_out;
        if (xQueueReceive(pid_queue, &pid_out, pdMS_TO_TICKS(100)) == pdPASS)
        {
            float control = pid_out.pitch_output;
            if (fabsf(control) < SERVO_PID_OUTPUT_DEADBAND)
            {
                control = 0.0f;
            }

            float target = PITCH_SERVO_NEUTRAL_DEG - (control * PITCH_SERVO_GAIN);
            servo_cmd_t cmd = {
                .target_angle = target,
                .immediate = false,
            };
            xQueueOverwrite(servo_cmd_queue, &cmd);

            trace_div++;
            if (trace_div >= 20U)
            {
                printf("[servo] pid control=%.2f target=%.2f\n", control, target);
                trace_div = 0U;
            }
        }
    }
}

void servo_set_pitch_pid_bypass(bool enabled)
{
    s_pitch_servo_pid_bypass = enabled;
    printf("Pitch servo mode: %s\n", enabled ? "DIRECT" : "PID");
}

bool servo_get_pitch_pid_bypass(void)
{
    return s_pitch_servo_pid_bypass;
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
    uint64_t perf_last_log_us = esp_timer_get_time();
    uint64_t perf_max_loop_us = 0;
    uint32_t perf_samples = 0;

    while (1)
    {
        uint64_t loop_start_us = esp_timer_get_time();

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

        uint64_t loop_us = esp_timer_get_time() - loop_start_us;
        if (loop_us > perf_max_loop_us)
        {
            perf_max_loop_us = loop_us;
        }

        perf_samples++;
        uint64_t now_us = esp_timer_get_time();
        if ((now_us - perf_last_log_us) >= SERVO_PERF_LOG_INTERVAL_US)
        {
            printf("SERVO TASK PERF samples=%lu max_loop_us=%llu\n",
                   (unsigned long)perf_samples,
                   (unsigned long long)perf_max_loop_us);
            perf_last_log_us = now_us;
            perf_max_loop_us = 0;
            perf_samples = 0;
        }

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

esp_err_t servo_stabilizer_start_pitch(void)
{
    BaseType_t task_ok;
    esp_err_t err;
    const app_config_t *cfg;

    if (s_pitch_servo_bridge_task != NULL)
    {
        return ESP_OK;
    }

    cfg = config_manager_get();
    if (cfg != NULL)
    {
        if (cfg->servo_min_us >= (int32_t)SERVO_MIN_PULSE_US_LIMIT && cfg->servo_min_us < cfg->servo_max_us)
        {
            s_pitch_servo.min_pulse_us = (float)cfg->servo_min_us;
        }
        if (cfg->servo_max_us <= (int32_t)SERVO_MAX_PULSE_US_LIMIT && cfg->servo_max_us > cfg->servo_min_us)
        {
            s_pitch_servo.max_pulse_us = (float)cfg->servo_max_us;
        }
    }

    if ((s_pitch_servo.max_pulse_us - s_pitch_servo.min_pulse_us) < SERVO_MIN_EFFECTIVE_SPAN_US)
    {
        s_pitch_servo.min_pulse_us = SERVO_MIN_PULSE_US_LIMIT;
        s_pitch_servo.max_pulse_us = SERVO_MAX_PULSE_US_LIMIT;
    }

    printf("Pitch servo PWM range: %.0fus..%.0fus, gain=%.2f\n",
           s_pitch_servo.min_pulse_us,
           s_pitch_servo.max_pulse_us,
           (double)PITCH_SERVO_GAIN);

    servo_init(&s_pitch_servo);
    err = servo_start_task(&s_pitch_servo);
    if (err != ESP_OK)
    {
        return err;
    }

    task_ok = xTaskCreate(
        pitch_servo_stabilizer_task,
        "pitch_servo_ctrl",
        3072,
        NULL,
        8,
        &s_pitch_servo_bridge_task);
    if (task_ok != pdPASS)
    {
        s_pitch_servo_bridge_task = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}
