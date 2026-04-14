#include "watchdog_service.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define MAX_WATCHDOG_TASKS 10 // max number of task that can be monitored by the watchdog
// structure representing a registered task in the watchdog system
typedef struct
{
    const char *name;  // human readable task name
    uint32_t timeout_ms; // timeout in milliseconds
    TickType_t last_feed; // last time the task fed the watchdog
    bool active; // whether this entry is active or not
} watchdog_entry_t;
// static array storing all registered tasks
static watchdog_entry_t tasks[MAX_WATCHDOG_TASKS];
// number of currently registered tasks
static int task_count = 0;
// flag to ensure the watchdog task is only started once
static bool s_watchdog_task_started = false;
// register a task with the watchdog system, returns a handle that can be used to feed the watchdog
watchdog_handle_t watchdog_register(const char *name, uint32_t timeout_sec)
{
    if (task_count >= MAX_WATCHDOG_TASKS)
    {
        return -1; // No more space for new tasks
    }

    tasks[task_count].name = name; // store the task name for debugging purposes
    tasks[task_count].timeout_ms = timeout_sec * 1000; // convert timeout to ms
    tasks[task_count].last_feed = xTaskGetTickCount(); // initialize last feed time to now
    tasks[task_count].active = true; // mark this entry as active

    return task_count++; // Return the handle and increment count
}
// feed the watchdog for a specific task using its handle
void watchdog_feed(watchdog_handle_t handle) 
{
    if (handle < 0 || handle >= task_count || !tasks[handle].active) // validate handle and check if entry is active
    {
        return;
    }

    tasks[handle].last_feed = xTaskGetTickCount();
}
// main watchdog task that runs in the background and checks for timeouts
void watchdog_task(void *arg)
{
    esp_task_wdt_add(NULL); // Add the watchdog task to the hardware watchdog
    while (1)
    {
        TickType_t now = xTaskGetTickCount(); // get the current tick count to calculate elapsed time for each task
        for (int i = 0; i < task_count; i++)
        {
            if (!tasks[i].active) // skip inactive entries
                continue;
            uint32_t elapsed = (now - tasks[i].last_feed) * portTICK_PERIOD_MS;
            if (elapsed > tasks[i].timeout_ms) // if the elapsed time exceeds the task's timeout, we have a watchdog timeout
            {
                printf("Watchdog timeout for task: %s\n", tasks[i].name);

                esp_restart();
            }
        }
        esp_task_wdt_reset(); // reset the hardware watchdog timer to prevent it from triggering a system reset
        vTaskDelay(pdMS_TO_TICKS(200)); // delay for a short period before checking again to avoid busy looping
    }
}
 // initialize the watchdog system with a specigied hardware timeout in S
 // this sets up the hardware watchdog and starts the task if it hasn't been started yet
void watchdog_init(uint32_t hw_timeout_sec)
{
    esp_err_t ret; // configure the hardware watchdog with the specified timeout and settings

    esp_task_wdt_config_t config = {
        .timeout_ms = hw_timeout_sec * 1000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true,
    };

    ret = esp_task_wdt_init(&config); // initialize the hardware watchdog with the specified config
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        printf("watchdog_init failed: %s\n", esp_err_to_name(ret));
        return;
    }

    if (!s_watchdog_task_started) // start the watchdog task if it hasn't been started yet to monitor registered tasks and feed the hardware watchdog
    {
        if (xTaskCreate(watchdog_task, "watchdog_task", 2048, NULL, 10, NULL) == pdPASS)
        {
            s_watchdog_task_started = true;
        }
    }
}