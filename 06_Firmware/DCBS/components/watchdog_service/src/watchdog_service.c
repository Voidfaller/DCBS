#include "watchdog_service.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define MAX_WATCHDOG_TASKS 10

typedef struct
{
    const char *name;
    uint32_t timeout_ms;
    TickType_t last_feed;
    bool active;
} watchdog_entry_t;

static watchdog_entry_t tasks[MAX_WATCHDOG_TASKS];
static int task_count = 0;

watchdog_handle_t watchdog_register(const char *name, uint32_t timeout_sec)
{
    if (task_count >= MAX_WATCHDOG_TASKS)
    {
        return -1; // No more space for new tasks
    }

    tasks[task_count].name = name;
    tasks[task_count].timeout_ms = timeout_sec * 1000;
    tasks[task_count].last_feed = xTaskGetTickCount();
    tasks[task_count].active = true;

    return task_count++; // Return the handle and increment count
}

void watchdog_feed(watchdog_handle_t handle)
{
    tasks[handle].last_feed = xTaskGetTickCount();
}

void watchdog_task(void *arg)
{
    esp_task_wdt_add(NULL); // Add the watchdog task to the hardware watchdog
    while (1)
    {
        TickType_t now = xTaskGetTickCount();
        for (int i = 0; i < task_count; i++)
        {
            if (!tasks[i].active)
                continue;
            uint32_t elapsed = (now - tasks[i].last_feed) * portTICK_PERIOD_MS;
            if (elapsed > tasks[i].timeout_ms)
            {
                printf("Watchdog timeout for task: %s\n", tasks[i].name);

                esp_restart();
            }
        }
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void watchdog_init(uint32_t hw_timeout_sec)
{
    esp_task_wdt_config_t config = {
        .timeout_ms = hw_timeout_sec * 1000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true,
    };

    esp_task_wdt_init(&config);
    xTaskCreate(watchdog_task, "watchdog_task", 2048, NULL, 10, NULL);
}