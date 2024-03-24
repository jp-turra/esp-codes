// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "esp_log.h"

#include "hal/gpio_types.h"
#include "driver/gpio.h"

#define TAG "blink_app"
#define TIMER_ID 1
#define BLINK_PERIOD 500 / portTICK_PERIOD_MS

// Application headers

// Global application variables
static gpio_num_t g_led_pin = 2;
static gpio_config_t g_led_config;

static uint8_t led_state = 0;

// Global NETIF variables

// Private/Static functions
static void blink_led()
{
    if (led_state == 0)
    {
        // ESP_LOGI(TAG, "LED ON");
        gpio_set_level(g_led_pin, 1);
    }
    else
    {
        // ESP_LOGI(TAG, "LED OFF");
        gpio_set_level(g_led_pin, 0);
    }
}

static void blink_led_task(void* arg)
{
    while (1)
    {
        blink_led();
        // vTaskDelay(BLINK_PERIOD);
        // led_state = !led_state;
    }
}

static void blink_increment_led_state(TimerHandle_t xTimer)
{
    configASSERT(xTimer);


    ESP_LOGI(TAG, "Increment LED state");
    uint8_t xtimer_id = (uint8_t) pvTimerGetTimerID(xTimer);
    if (TIMER_ID == xtimer_id)
    {
        ESP_LOGI(TAG, "Increment LED state");
        led_state = !led_state;
    }
    else
    {
        ESP_LOGW(TAG, "Invalid timer ID");
    }
}

static void configure_led()
{
    g_led_config.intr_type = GPIO_INTR_DISABLE;
    g_led_config.mode = GPIO_MODE_OUTPUT;
    g_led_config.pin_bit_mask = (1 << g_led_pin);
    g_led_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    g_led_config.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&g_led_config);
    ESP_LOGI(TAG, "Led configured");
}

static void configure_timer()
{
    TimerHandle_t timer = xTimerCreate("led_timer", BLINK_PERIOD, pdTRUE, 
                                        (void *) TIMER_ID, blink_increment_led_state);

    if (timer == NULL)
    {
        ESP_LOGE(TAG, "Failed to create timer");
        abort();
    }
    else 
    {
        ESP_LOGI(TAG, "Timer created");
        if (xTimerStart(timer, 0) != pdPASS)
        {
            ESP_LOGE(TAG, "Failed to start timer");
            abort();
        }
    }
}

// Public functions

void app_main(void)
{
    configure_led();

    configure_timer();

    ESP_LOGI(TAG, "Creating blink led task");
    // Note: High priority task must be executed fast enough to not activate the watchdog.
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
    xTaskCreate(blink_led_task, "blink_led", 2048, NULL, 0, NULL); // 2048 bytes alocated for the task

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());
}
