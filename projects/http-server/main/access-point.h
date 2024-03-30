#ifndef ACCESS_POINT_HTTP_SERVER__H
#define ACCESS_POINT_HTTP_SERVER__H

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

#include "esp_log.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "nvs_flash.h"
#include "nvs.h"

#define WIFI_AP_SSID                "ESP32_AP"
#define WIFI_AP_CHANNEL             5               // 1 - 13 | 20MHz channels
#define WIFI_AP_SSID_HIDDEN         0
#define WIFI_AP_PASS                "rockit123"
#define WIFI_AP_MAX_CLIENTS         2
#define WIFI_AP_BEACON_INTERVAL     100             // 100ms
#define WIFI_AP_IP                  "192.168.0.1"   // AP Default IP
#define WIFI_AP_GATEWAY             "192.168.0.1"   // AP Default Gateway (should be the same as AP IP)
#define WIFI_AP_NETMASK             "255.255.255.0" // AP Default Netmask
#define WIFI_AP_BANDWIDTH           WIFI_BW_HT20    // WIFI_BW_HT20 (20MHz -> 72Mbps) | WIFI_BW_HT40 (40MHz -> 150Mbps)
#define WIFI_STA_POWER_SAFE         WIFI_PS_NONE    // WIFI_PS_MIN_MODEM | WIFI_PS_MAX_MODEM | WIFI_PS_NONE
#define MAX_SSID_LENGTH             32              // IEEE Maximum SSID Length
#define MAX_PASS_LENGTH             64              // IEEE Maximum Password Length
#define MAX_CONN_RETRIES            5               // Retry number on disconnect

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

// Structure for message queue for the wifi application task
typedef struct wifi_app_queue_msg
{
    wifi_app_message_e msg_id;
} wifi_app_queue_message_t;

// Sends a message to the queue of the wifi application
BaseType_t wifi_app_send_message(wifi_app_message_e msg_id);

// Event handler for Wifi application
static void wifi_app_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// Initializes the Wifi application event handler for Wifi an Ip events
static void wifi_app_event_handler_init(void);

// Initializes the TCP stack and default Wifi config
static void wifi_app_default_wifi_init(void);

// Configures Wifi access point settings and assigns the static IP to the SoftAP
static void wifi_app_soft_ap_config(void);

// Main task for wifi application
static void wifi_app_task(void* pvParameters);

// Start the RTOS Wifi Task
void wifi_app_start(void);

// Intialize NVS
void wifi_app_initialize_nvs(void);

#endif // !ACCESS_POINT_HTTP_SERVER__H