#include <stdio.h>
#include "servo_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "watchdog_service.h"

void app_main(void)
{
    watchdog_init(5); // Initialize watchdog with 5-second hardware timeout

    while (1)
    {
        
    }
}
