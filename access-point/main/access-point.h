#ifndef ACCESS_POINT__H
#define ACCESS_POINT__H

// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF headers
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "hal/gpio_types.h"
#include "driver/gpio.h"

#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#define WIFI_AP_SSID "ESP32_AP"
#define WIFI_AP_CHANNEL 5                   // 1 - 13 | 20MHz channels
#define WIFI_AP_SSID_HIDDEN 0
#define WIFI_AP_PASS "rockit123"
#define WIFI_AP_MAX_CLIENTS 2
#define WIFI_AP_BEACON_INTERVAL 100         // 100ms
#define WIFI_AP_IP "192.168.0.1"            // AP Default IP
#define WIFI_AP_GATEWAY "192.168.0.1"       // AP Default Gateway (should be the same as AP IP)
#define WIFI_AP_NETMASK "255.255.255.0"     // AP Default Netmask
#define WIFI_AP_BANDWIDTH WIFI_BW_HT20      // WIFI_BW_HT20 (20MHz -> 72Mbps) | WIFI_BW_HT40 (40MHz -> 150Mbps)
#define WIFI_STA_POWER_SAFE WIFI_PS_NONE    // WIFI_PS_MIN_MODEM | WIFI_PS_MAX_MODEM | WIFI_PS_NONE
#define MAX_SSID_LENGTH 32                  // IEEE Maximum SSID Length
#define MAX_PASS_LENGTH 64                  // IEEE Maximum Password Length
#define MAX_CONN_RETRIES 5                  // Retry number on disconnect

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

// Global NETIF variables
// Net Interface objectos for station and access point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

// Messages ID for the wifi application task
typedef enum wifi_app_message
{
    WIFI_APP_MESSAGE_START_HTTP_SERVER = 0,
    WIFI_APP_MESSAGE_CONNECTING_FROM_HTTP_SERVER,
    WIFI_APP_MESSAGE_STA_CONNECTED_GOT_IP,

} wifi_app_message_e;

// Structure for message queue
typedef struct wifi_app_queue_msg
{
    wifi_app_message_e msg_id;
} wifi_app_queue_message_t;

// Sends a message to the queue
BaseType_t wifi_app_send_message(wifi_app_message_e msg_id);

// Start the RTOS Wifi Task
void wifi_app_start(void);

// LED functions
static void configure_led();
static void led_on();
static void led_off();
static void blink_wifi_configuring();
static void blink_wifi_connected();
static void blink_wifi_error();
static void blink_wifi_disconnected();
static void blink_increment_led_state();
static void blink_wifi_task(void* args);



#endif // !ACCESS_POINT__H

