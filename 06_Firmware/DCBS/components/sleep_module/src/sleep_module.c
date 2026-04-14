/*#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "driver/rtc_io.h"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // 2 ^ GPIO_NUMBER in hex
#define USE_EXT0_WAKEUP 0                        // 1 = EXT0 wakeup, 0 = EXT1 wakeup
#define WAKEUP_GPIO_1 GPIO_NUM_32                  // Only RTC IO are allowed - ESP32 Pin example
#define WAKEUP_GPIO_2 GPIO_NUM_33
#define MAX_SLEEP_TIMER_TASKS 2
#define TIMEOUT_MS 30000

typedef struct
{
    const char name;
    uint32_t time_until_sleep;
    TickType_t last_awake_call;
    bool active;
} sleep_timer_entry;

static sleep_timer_entry tasks[MAX_SLEEP_TIMER_TASKS];
static int task_count = 0;

sleep_timer_handle_t sleep_timer_register(const char *name, uint32_t stay_awake_sec)
{
    if (task_count >= SLEEP_TIMER_TASKS)
    {
        return -1; // No more space for new tasks
    }

    tasks[task_count].name = name;
    tasks[task_count].time_until_sleep = stay_awake_sec * 1000;
    tasks[task_count].last_awake_call = xTaskGetTickCount();
    tasks[task_count].active = true;

    return task_count++; // Return the handle and increment count
}

void sleep_timer_reset()
{
    tasks[1].last_reset = xTaskGetTickCount();
    tasks[2].last_reset = xTaskGetTickCount();
}


void sleep_timer_task(void *arg)
{
    esp_task_wdt_add(NULL); // Add the sleep timer task
    while(1){
        TickType_t now = xTaskGetTickCount();
        for (int i = 0; i < task_count; i++){
            uint32_t elapsed = (now - timer.last_awake_call) * portTICK_PERIOD_MS;
            if (elapsed > sleep_timer.time_until_sleep){
                esp_deep_sleep_start();
            }
        }
        esp_task_wdt_reset();           
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void sleep_timer_init(uint32_t time_until_sleep_sec)
{
    esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO_1, 1);  //1 = High, 0 = Low
    esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO_2, 1);
    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    rtc_gpio_pullup_dis(WAKEUP_GPIO_1);
    rtc_gpio_pulldown_en(WAKEUP_GPIO_1);
    rtc_gpio_pullup_dis(WAKEUP_GPIO_2);
    rtc_gpio_pulldown_en(WAKEUP_GPIO_2);

    esp_task_wdt_config_t config = {
        .time_until_sleep = time_until_sleep_sec * 1000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true,
    };

    esp_task_wdt_init(&config);
    xTaskCreate(sleep_timer_task, "sleep_timer_task", 2048, NULL, 10, NULL);

    esp_task_wdt_config_t config = {
        .time_until_sleep = TIMEOUT_MS,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true,
    };

    esp_task_wdt_init(&config);
    xTaskCreate(sleep_timer_task, "sleep_timer_timeout_task", 2048, NULL, 10, NULL);
}
    */

#include "energy_manager.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/touch_pad.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <stdio.h>

#define TAG "ENERGY"

#define MAX_CLIENTS 10
#define TASK_STACK 2048 
#define TASK_PRIO 8

#define TOUCH_PAD_USED TOUCH_PAD_NUM0

typedef struct 
{
    const char *name;
    uint64_t last_activity_us;
    bool active;
} energy_client_t;

static energy_client_t clients[MAX_CLIENTS];
static int client_count = 0;

static QueueHandle_t s_queue = NULL;
static TaskHandle_t s_task = NULL;

static uint64_t s_timeout_us = 0;
static void energy_prepare_sleep(void) 
{
    ESP_LOGI(TAG, "preparing to sleep...");
    // integrate the modules 


    touch_pad_init();
    touch_pad_config(TOUCH_PAD_USED, 0);
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    toùch_pad_filter_start(10);
    touch_pad_set_tresh(TOUCH_PAD_USED, 40);
    esp_sleep_enable_touchpad_wakeup();
esp_deep_sleep_start();

}

static bool energy_should_sleep(void)
{
    uint64_t now = esp_timer_get_time();
    for (int i = 0; i<client_count; i++)
    {
        if (!clients[i].active)
        continue;
        uint64_t elapsed = now - clients[i].last_actiivity_us;
        if (elapsed < s_timeout_us)
        {
            return false;
        }
    return true;
    }
 

static void energy_task(void *arg)
{
    energy_event_t event;
    while(1)
    {
        if (xQueueReceive(s_queue, &event, pdMS_TO_TICKS(200)) == pdPASS)
        {
            if (event == ENERGY_EVENT_FORCE_SLEEP)
            {
                energy_prepare_sleep();
                energy_enter_sleep();
            }
        }
        if (energy_should_sleep())       
        {
            ESP_LOGI(TAG, "no activity, time to sleeeeep");
            energy_prepare_sleep();
            energy_enter_sleep();
        }
    }
}

esp_err_t energy_manager_init(uint32_t timeout_sec){
    s_timeout_us = timeout_sec * 1000000ULL;
    if (s_queue == NULL)
    {
        s_queue = xQueueCreate(4, sizeof(energy_event_t));
        if (s_queue == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
        if(s_task == NULL)
        {
            if (xtaskCreate(energy_task, "energy_task, TASK_STACK, NULL, TASK_PRIO, &s_task"))
            {
                return ESP_FAIL;
            }
            return ESP_OK;
        }

        energy_handle_t energy_manager_register(const char *name)
        {
            if (client_count >= MAX_CLIENTS)
                 return -1;
            clients[client_count].name = name;
            clients[client_count].last_activity_us = esp_timer_get_time();
            clients[client_count].active = true;
            return client_count++;
        }

void energy_manager_feed(energy_handle_t handle)
{
    if (handle < 0 || handle >= client_count)
        return;

    clients[handle].last_activity_us = esp_timer_get_time();
}

QueueHandle_t energy_manager_get_queue(void)
{
    return s_queue;
}

void energy_manager_check_wakeup(void)
{
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    switch (cause)
    {
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            ESP_LOGI(TAG, "wakeup cause touch");
            break;

        case ESP_SLEEP_WAKEUP_TIMER:
            ESP_LOGI(TAG, "wakeuuup: TIMER");
            break;

        default:
            ESP_LOGI(TAG, "normal boot");
            break;
    }
}    }


}

}

