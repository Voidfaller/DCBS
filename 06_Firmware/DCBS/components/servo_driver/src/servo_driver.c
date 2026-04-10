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
#include "watchdog_service.h"

static watchdog_handle_t s_watchdog_handle = WATCHDOG_INVALID_HANDLE;
static bool s_pitch_pid_bypass = false;
static TaskHandle_t s_servo_task_handle = NULL;

void servo_init(servo_t *servo)
{
    if (servo == NULL)
    {
        return;
    }

    // Configure the LEDC timer for the servo
    ledc_timer_config_t timer_conf = {
        .speed_mode = servo->speed_mode,
        .timer_num = servo->timer_num,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .freq_hz = 50,               // Standard servo frequency
        .clk_cfg = LEDC_USE_APB_CLK, // Use APB clock (80/160MHz) for better stability
    };
    esp_err_t timer_err = ledc_timer_config(&timer_conf);
    if (timer_err != ESP_OK)
    {
        printf("servo timer config failed: %s\n", esp_err_to_name(timer_err));
        return;
    }

    // Configure the LEDC channel for the servo
    ledc_channel_config_t channel_conf = {
        .gpio_num = servo->gpio_pin,
        .speed_mode = servo->speed_mode,
        .channel = servo->channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = servo->timer_num,
        .duty = 0, // Start with 0 duty (servo off)
        .hpoint = 0,
    };
    esp_err_t channel_err = ledc_channel_config(&channel_conf);
    if (channel_err != ESP_OK)
    {
        printf("servo channel config failed: %s\n", esp_err_to_name(channel_err));
    }
}

void servo_set_pwm(servo_t *servo, int pwm)
{
    if (servo == NULL)
    {
        return;
    }

    if (pwm < 500)
    {
        pwm = 500;
    }
    else if (pwm > 2500)
    {
        pwm = 2500;
    }

    // Convert microseconds to duty cycle value for 14-bit timer at 50Hz (20ms period)
    const uint32_t max_duty = (1U << 14) - 1U;
    uint32_t duty = (uint32_t)(((uint64_t)pwm * max_duty) / 20000ULL);
    ledc_set_duty(servo->speed_mode, servo->channel, duty);
    ledc_update_duty(servo->speed_mode, servo->channel);
}

void servo_sweep_test(servo_t *servo)
{
    if (servo == NULL)
    {
        return;
    }

    // Sweep from min to max
    servo_set_pwm(servo, 500);      // Move to min
    vTaskDelay(pdMS_TO_TICKS(500)); // Pause at min
    servo_set_pwm(servo, 2500);     // Move to max
    vTaskDelay(pdMS_TO_TICKS(500)); // Pause at max
    servo_set_pwm(servo, 1500);     // Move back to neutral
    vTaskDelay(pdMS_TO_TICKS(500)); // Pause at neutral
}

int clamp(int value, int min, int max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}
void servo_task(void *arg)
{
    bool bypass_pid = *(bool *)arg;
    // This function can be used to implement a control loop for the servo if needed.
    // For now, it just deletes itself.

    orientation_t orientation = {0};
    servo_t pitch_servo = {
        .gpio_pin = 18,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_1, // Use different timer to avoid conflicts
        .channel = LEDC_CHANNEL_1,
    };

    servo_t roll_servo = {
        .gpio_pin = 5,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_2, // Use different timer to avoid conflicts
        .channel = LEDC_CHANNEL_2,
    };

    QueueHandle_t orientation_queue = state_estimator_get_queue_handle();
    QueueHandle_t gyro_monitor_queue = gyro_driver_get_monitor_queue_handle();
    if (orientation_queue != NULL)
    {
        printf("[test] state estimator queue is available\n");
    }
    else
    {
        printf("[test] state estimator queue is NOT available\n");
    }

    servo_init(&pitch_servo);
    servo_init(&roll_servo);
    servo_sweep_test(&pitch_servo);
    servo_sweep_test(&roll_servo);
    while (1)
    {
        // pid bypassed
        if (s_pitch_pid_bypass)
        {
            if (xQueueReceive(orientation_queue, &orientation, pdMS_TO_TICKS(100)) == pdPASS)
            {

                int pitch_pwm = (int)(1500.0f + (orientation.pitch * 13.90f));
                int roll_pwm = (int)(1500.0f - (orientation.roll * 13.90f));

                pitch_pwm = clamp(pitch_pwm, 500, 2500);
                roll_pwm = clamp(roll_pwm, 500, 2500);

                printf("[test] est=(pitch=%.2f roll=%.2f) pwm=%d\n",
                       orientation.pitch,
                       orientation.roll,
                       pitch_pwm);

                servo_set_pwm(&pitch_servo, pitch_pwm);
                servo_set_pwm(&roll_servo, roll_pwm);
            }
        }

        // pid control loop
        else
        {
            
        }

        watchdog_feed(s_watchdog_handle);
    }

    vTaskDelete(NULL);
}

esp_err_t servo_start_task(bool bypass_pid)
{

    s_pitch_pid_bypass = bypass_pid;
    s_watchdog_handle = watchdog_register("Servo Driver", 5);
    BaseType_t task_ok = xTaskCreate(servo_task, "Servo Task", 2048, &bypass_pid, 10, &s_servo_task_handle);
    if (task_ok != pdPASS)
    {
        s_servo_task_handle = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}