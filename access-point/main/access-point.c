// Application headers
#include "access-point.h"

// Private/Static functions
static void configure_led()
{
    g_led_config.intr_type = GPIO_INTR_DISABLE;
    g_led_config.mode = GPIO_MODE_OUTPUT;
    g_led_config.pin_bit_mask = (1 << g_led_pin);
    g_led_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    g_led_config.pull_up_en = GPIO_PULLUP_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&g_led_config));
    ESP_LOGI(TAG, "Led configured");
}

static void led_on()
{
    gpio_set_level(g_led_pin, 1);
}

static void led_off()
{
    gpio_set_level(g_led_pin, 0);
}

static void blink_wifi_configuring()
{
    if (g_led_config_state == 0)
    {
        led_on();
        vTaskDelay(250 / portTICK_PERIOD_MS);
        
        led_off();
        vTaskDelay(250 / portTICK_PERIOD_MS);

        g_led_config_state = 1;
    }
    else if (g_led_config_state == 1)
    {
        led_on();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        
        led_off();
        vTaskDelay(500 / portTICK_PERIOD_MS);

        g_led_config_state = 0;
    }
    
}

static void blink_wifi_connected()
{
    led_on();
}

static void blink_wifi_error()
{
    led_on();
    vTaskDelay(g_delay_ms);
    led_off();
    vTaskDelay(g_delay_ms);
}

static void blink_wifi_disconnected()
{
    led_off();
}

static void blink_wifi_task(void* args)
{
    while (1)
    {
        if (g_led_state == LED_WIFI_CONFIGURING)
        {
            blink_wifi_configuring();
        }
        else if (g_led_state == LED_WIFI_CONNECTED)
        {
            blink_wifi_connected();
        }
        else if (g_led_state == LED_WIFI_ERROR)
        {
            blink_wifi_error();
        }
        else if (g_led_state == LED_WIFI_DISCONNECT)
        {
            blink_wifi_disconnected();
        }
    }
}

static void blink_increment_led_state()
{
    if (g_led_state == LED_WIFI_ERROR)
    {
        g_led_state = LED_WIFI_DISCONNECT;
    }

    g_led_state ++;

    switch (g_led_state)
    {
    case LED_WIFI_ERROR:
        ESP_LOGI(TAG, "Led state: LED_WIFI_ERROR");
        break;

    case LED_WIFI_DISCONNECT:
        ESP_LOGI(TAG, "Led state: LED_WIFI_DISCONNECT");
        break;

    case LED_WIFI_CONNECTED:
        ESP_LOGI(TAG, "Led state: LED_WIFI_CONNECTED");
        break;

    case LED_WIFI_CONFIGURING:
        ESP_LOGI(TAG, "Led state: LED_WIFI_CONFIGURING");
        break;
    
    default:
        break;
    }
}

static void configure_timer()
{
    TimerHandle_t timer = xTimerCreate("led_timer", g_change_led_state_ms, pdTRUE, 
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

    ESP_LOGI(TAG, "Creating wifi blink task");
    // Note: High priority task must be executed fast enough to not activate the watchdog.
    xTaskCreate(blink_wifi_task, "Blink Wifi Status Task", 4096, NULL, tskIDLE_PRIORITY, NULL);
}
