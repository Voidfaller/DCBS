#include "sleep_module.h"
#include "freertos/FreeRTOS.h"
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

static sleep_timer_entry_t tasks[MAX_SLEEP_TIMER_TASKS];
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