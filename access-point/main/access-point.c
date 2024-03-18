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

// Application headers

// Global application variables
static gpio_num_t g_led_pin = 2;
static gpio_config_t g_led_config;
static uint8_t led_state = 0;

// Global NETIF variables

// Private/Static functions
static void configure_led()
{
    g_led_config.intr_type = GPIO_INTR_DISABLE;
    g_led_config.mode = GPIO_MODE_OUTPUT;
    g_led_config.pin_bit_mask = (1 << g_led_pin);
    g_led_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    g_led_config.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&g_led_config);
}

static led_on()
{
    printf("LED ON\n");
    gpio_set_level(g_led_pin, 1);
}

static led_off()
{
    printf("LED OFF\n");
    gpio_set_level(g_led_pin, 0);
}

static void blink_wifi_configuring()
{
    if (led_state == 0)
    {
        led_on();
        vTaskDelay(250 / portTICK_PERIOD_MS);
        
        led_off();
        vTaskDelay(250 / portTICK_PERIOD_MS);

        led_state ++;
    }
    else if (led_state == 1)
    {
        led_on();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        
        led_off();
        vTaskDelay(500 / portTICK_PERIOD_MS);

        led_state = 0;
    }
    
}

static void blink_wifi_connected()
{
    led_on();
    vTaskDelay(250 / portTICK_PERIOD_MS);
}

static void blink_wifi_error()
{
    led_on();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    led_off();
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

// Public functions

void app_main(void)
{
    configure_led();
}
