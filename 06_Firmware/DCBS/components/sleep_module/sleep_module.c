#include "sleep_module.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_log.h"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // 2 ^ GPIO_NUMBER in hex
#define USE_EXT0_WAKEUP 0                        // 1 = EXT0 wakeup, 0 = EXT1 wakeup
#define WAKEUP_GPIO_1 GPIO_NUM_4                  // Only RTC IO are allowed - ESP32 Pin example
#define TIMEOUT_US 30000000

esp_timer_handle_t timeout_timer;
esp_timer_handle_t sleep_timer;

void sleep_timer_reset(uint32_t time_until_sleep_sec)
{
    esp_timer_restart(sleep_timer, time_until_sleep_sec*1000000);
    esp_timer_restart(timeout_timer, TIMEOUT_US);
}

void sleep_start()
{
    printf("Going to sleep now");
    esp_deep_sleep_start();
}

esp_err_t sleep_timer_init(uint32_t time_until_sleep_sec)
{
    esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO_1, 1); //1 = High, 0 = Low
    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    rtc_gpio_pullup_dis(WAKEUP_GPIO_1);
    rtc_gpio_pulldown_en(WAKEUP_GPIO_1);

    esp_timer_create_args_t timer_config = {
        .callback = &sleep_start,
        .name = "sleep_timer",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_config, &sleep_timer));

    esp_timer_create_args_t timeout_config = {
        .callback = &sleep_start,
        .name = "timeout_timer",
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timeout_config, &timeout_timer));
    uint32_t time_until_sleep_us = time_until_sleep_sec * 1000000;
    ESP_ERROR_CHECK(esp_timer_start_once(timeout_timer, TIMEOUT_US));
    ESP_ERROR_CHECK(esp_timer_start_once(sleep_timer, time_until_sleep_us));

    return ESP_OK;
}