// Application headers
#include "access-point.h"

static void blink_increment_led_state();

// Private/Static functions
static void configure_timer()
{
    // Steps to configure timer
    // 1. Timer initialization
    timer_config_t config;
    timer_get_config(TIMER_GROUP_0, TIMER_0, &config);

    // Setting diver to 1 second. Considering default 80MHz clock source
    config.divider = 80e6;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_START;
    config.alarm_en = TIMER_ALARM_DIS;
    config.auto_reload = TIMER_AUTORELOAD_EN;

    // Initialize Timer
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    // 2. Timer Control
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

    // 3. Alarms (Disabeld)
    // 4. Interrupts
    
}

static void configure_led()
{
    g_led_config.intr_type = GPIO_INTR_DISABLE;
    g_led_config.mode = GPIO_MODE_OUTPUT;
    g_led_config.pin_bit_mask = (1 << g_led_pin);
    g_led_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    g_led_config.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&g_led_config);
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

        g_led_config_state ++;
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

static void blink_wifi_task()
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

    printf("g_led_state: %d\n", g_led_state);
}

// Public functions

void app_main(void)
{
    configure_led();

    configure_timer();

    xTaskCreate(blink_wifi_task, "blink_wifi_task", 4096, NULL, 5, NULL);
}
