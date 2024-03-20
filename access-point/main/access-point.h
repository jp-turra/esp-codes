#ifndef ACCESS_POINT__H
#define ACCESS_POINT__H

// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "hal/gpio_types.h"

#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/timer.h"

#include "esp_log.h"

// Global application variables
static enum e_led_state
{
    LED_WIFI_DISCONNECT=0,
    LED_WIFI_CONFIGURING,
    LED_WIFI_CONNECTED,
    LED_WIFI_ERROR
} g_led_state = LED_WIFI_DISCONNECT;

static gpio_num_t g_led_pin = 2;
static gpio_config_t g_led_config;
static uint8_t g_led_config_state = 0;
static TickType_t g_delay_ms = 100 / portTICK_PERIOD_MS; // 100ms
static TickType_t g_change_led_state_ms = 2000 / portTICK_PERIOD_MS; // 2 s
static TickType_t g_last_tick_ms = portTICK_PERIOD_MS;

typedef struct {
    uint32_t event_count;
} queue_element_t;

// Global NETIF variables

#endif // !ACCESS_POINT__H

