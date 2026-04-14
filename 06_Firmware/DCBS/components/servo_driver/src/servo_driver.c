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

// used for direct angle to pwm mapping 
#define PITCH_SERVO_SENSITIVITY 13.90f 
#define ROLL_SERVO_SENSITIVITY 13.90f  

// watchdog handle for servo task
static watchdog_handle_t s_watchdog_handle = WATCHDOG_INVALID_HANDLE;
// flag to bypass PID control and directly control servos based on orentation
static bool s_pitch_pid_bypass = false;
static TaskHandle_t s_servo_task_handle = NULL;
QueueHandle_t s_pid_queue_handle = NULL; // Expose PID queue handle for testing

// initalize a servo using esp32 LEDC PWM hardware
void servo_init(servo_t *servo)
{
    if (servo == NULL)
    {
        return;
    }

    // Configure the LEDC timer for the servo
    ledc_timer_config_t timer_conf = { // configure timer for 50hz PWM signal with 14bit resolution
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
// set servo PWM in microseconds and clamp to valid range 
void servo_set_pwm(servo_t *servo, int pwm, int correction)
{   
    int corrected_pwm;

    if (servo == NULL)
    {
        return;
    }
    // apply correction 
    corrected_pwm = pwm + correction;
    // clamp to valid servo range
    if (corrected_pwm < 500)
    {
        corrected_pwm = 500;
    }
    else if (corrected_pwm > 2500)
    {
        corrected_pwm = 2500;
    }

    // Convert microseconds to duty cycle value for 14-bit timer at 50Hz (20ms period)
    const uint32_t max_duty = (1U << 14) - 1U;
    uint32_t duty = (uint32_t)(((uint64_t)corrected_pwm * max_duty) / 20000ULL);
    ledc_set_duty(servo->speed_mode, servo->channel, duty);
    ledc_update_duty(servo->speed_mode, servo->channel);
}
// simple test function to sweep servo from min to max and back to neutral to verify basic forward
void servo_sweep_test(servo_t *servo)
{
    if (servo == NULL)
    {
        return;
    }

    // Sweep from min to max
    servo_set_pwm(servo, 500, 0);      // Move to min
    vTaskDelay(pdMS_TO_TICKS(500)); // Pause at min
    servo_set_pwm(servo, 2500, 0);     // Move to max
    vTaskDelay(pdMS_TO_TICKS(500)); // Pause at max
    servo_set_pwm(servo, 1500, 0);     // Move back to neutral
    vTaskDelay(pdMS_TO_TICKS(500)); // Pause at neutral
}
// helper function to clamp values to a specified range
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

// main servo control task
void servo_task(void *arg)
{
    (void)arg;
    // This function can be used to implement a control loop for the servo if needed.
    // For now, it just deletes itself.
    // local orientation struct (currently unused in active mode)
    orientation_t orientation = {0};
    servo_t pitch_servo = {
        .gpio_pin = 18,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_1, // Use different timer to avoid conflicts
        .channel = LEDC_CHANNEL_1, // use different channel to avoid conflicts
    };
    // local roll servo struct
    servo_t roll_servo = {
        .gpio_pin = 5,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_2, // Use different timer to avoid conflicts
        .channel = LEDC_CHANNEL_2,
    };
    // check if state estimator queue is available for testing purposes
    QueueHandle_t orientation_queue = state_estimator_get_queue_handle();
    if (orientation_queue != NULL)
    {
        printf("[test] state estimator queue is available\n");
    }
    else
    {
        printf("[test] state estimator queue is NOT available\n");
    }
    // initialize servos
    servo_init(&pitch_servo);
    servo_init(&roll_servo);

    // Force neutral before calibration starts.
    servo_set_pwm(&pitch_servo, 1500, 0);
    servo_set_pwm(&roll_servo, 1500, 0);

    // Hold neutral while the state estimator performs zero calibration.
    while (!state_estimator_is_calibrated())
    {
        servo_set_pwm(&pitch_servo, 1500, 0);
        servo_set_pwm(&roll_servo, 1500, 0);
        watchdog_feed(s_watchdog_handle);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    printf("[servo] State estimator calibrated, enabling control loop\n");

    // servo_sweep_test(&pitch_servo);
    // servo_sweep_test(&roll_servo);
    while (1)
    {
        // pid bypassed
        if (s_pitch_pid_bypass)
        {
            // if (xQueueReceive(orientation_queue, &orientation, pdMS_TO_TICKS(100)) == pdPASS)
            // {

            //     int pitch_pwm = (int)(1500.0f + (orientation.pitch * 13.90f));
            //     int roll_pwm = (int)(1500.0f - (orientation.roll * 13.90f));

            //     pitch_pwm = clamp(pitch_pwm, 500, 2500);
            //     roll_pwm = clamp(roll_pwm, 500, 2500);

            //     printf("[test] est=(pitch=%.2f roll=%.2f) pwm=%d\n",
            //            orientation.pitch,
            //            orientation.roll,
            //            pitch_pwm);

                servo_set_pwm(&pitch_servo, 1500, 0);
                servo_set_pwm(&roll_servo, 1500, 0);
            }
        // }

        // pid control loop
        else
        {
            pid_correction_t correction; // struct to hold PID corrections for pitch and roll
            if (xQueueReceive(s_pid_queue_handle, &correction, pdMS_TO_TICKS(10)) == pdPASS) // receive latest PID correction from the queue
            {
                servo_set_pwm(&pitch_servo, 1500, correction.pitch_pwm * 1000); //scale correction to microseconds (assuming correction is in range [-1, 1])
                servo_set_pwm(&roll_servo, 1500, correction.roll_pwm * 1000); //scale correction to microseconds (assuming correction is in range [-1, 1])
            }
        }

        watchdog_feed(s_watchdog_handle); // feed the watchdog to prevent reset
    }

    vTaskDelete(NULL); // delete task if it ever exits the loop
}
// function to start the servo control task, takes a flag to bypass PID control for testing  
esp_err_t servo_start_task(bool bypass_pid)
{

    s_pitch_pid_bypass = bypass_pid; // set global flag to control PID bypass mode

    if (s_pid_queue_handle == NULL)
    {
        s_pid_queue_handle = pid_control_get_queue_handle(); // get the PID queue handle from the PID control module
    }

    s_watchdog_handle = watchdog_register("Servo Driver", 5); // register a watchdog with a 5 s timeout for servo task
    BaseType_t task_ok = xTaskCreate(servo_task, "Servo Task", 2048, &bypass_pid, 10, &s_servo_task_handle); // create the servo control task with the bypass flag as a parameter
    if (task_ok != pdPASS)
    {
        s_servo_task_handle = NULL; // reset task handle on failure
        return ESP_FAIL; // return failure if task creation failer
    }

    return ESP_OK; 
}