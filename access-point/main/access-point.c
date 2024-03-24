// Application headers
#include "access-point.h"
#include "task_common.h"

#include "esp_log.h"
#include "esp_err.h"

#include "nvs_flash.h"
#include "nvs.h"

static const char TAG[] = "access-point";

static QueueHandle_t wifi_app_queue_handle;

esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;

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

// Event handler for Wifi application
static void wifi_app_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
                break;

            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
                break;

            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
                break;

            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
                break;

            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CODISNNECTED");
                break;
            
            default:
                break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
                break;
            
            default:
                break;
        }
    }
}

// Initializes the Wifi application event handler for Wifi an Ip events
static void wifi_app_event_handler_init(void)
{
    // Event loop for wifi driver
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Event handler for the connection
    esp_event_handler_instance_t instance_wifi_event;
    esp_event_handler_instance_t instance_ip_event;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_wifi_event));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_ip_event));
}

// Initializes the TCP stack and default Wifi config
static void wifi_app_default_wifi_init(void)
{
    // Initialize the TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Default Wifi config - Operation must be in this order
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    esp_netif_sta = esp_netif_create_default_wifi_sta();
    esp_netif_ap = esp_netif_create_default_wifi_ap();
}

// Configures Wifi access point settings and assigns the static IP to the SoftAP
static void wifi_app_soft_ap_config(void)
{
    // Soft Ap - Wifi Access Point configuration
    wifi_config_t ap_config = 
    {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .password = WIFI_AP_PASS,
            .channel = WIFI_AP_CHANNEL,
            .ssid_hidden = WIFI_AP_SSID_HIDDEN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = WIFI_AP_MAX_CLIENTS,
            .beacon_interval = WIFI_AP_BEACON_INTERVAL
        }
    };

    // Configure DHCP for Access Point
    esp_netif_ip_info_t ap_ip_info;
    memset(&ap_ip_info, 0, sizeof(ap_ip_info));

    esp_netif_dhcps_stop(esp_netif_ap);             // Must call first
    inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip); // Assign Access Point's static IP, Gateway and Netmask
    inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
    inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info)); // Static configures network interface
    ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap)); // Starts the AP DHCP server for connection stations (like you phone)

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_AP_BANDWIDTH)); 
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAFE));
}

// Main task for wifi application
static void wifi_app_task(void* pvParameters)
{
    wifi_app_queue_message_t msg;

    // Initialize event handler
    wifi_app_event_handler_init();

    // Initilizar the TCP/IP stack and wifi config
    wifi_app_default_wifi_init();

    // SoftAP Config
    wifi_app_soft_ap_config();

    // Start wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    // Send fist event message 
    wifi_app_send_message(WIFI_APP_MESSAGE_START_HTTP_SERVER);

    for(;;)
    {
        if (xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY))
        {
            switch (msg.msg_id)
            {
            case WIFI_APP_MESSAGE_START_HTTP_SERVER:
                ESP_LOGI(TAG, "WIFI_APP_MESSAGE_START_HTTP_SERVER");

                // http_server_start();
                g_led_state = LED_WIFI_CONFIGURING;
                break;
            
            case WIFI_APP_MESSAGE_CONNECTING_FROM_HTTP_SERVER:
                ESP_LOGI(TAG, "WIFI_APP_MESSAGE_CONNECTING_FROM_HTTP_SERVER");

                g_led_state = LED_WIFI_CONFIGURING;
                break;

            case WIFI_APP_MESSAGE_STA_CONNECTED_GOT_IP:
                ESP_LOGI(TAG, "WIFI_APP_MESSAGE_STA_CONNECTED_GOT_IP");
                g_led_state = LED_WIFI_CONNECTED;
                break;
            
            default:
                break;
            }
        }
    }
}

void wifi_app_start(void)
{
    ESP_LOGI(TAG, "Starting wifi app task");
    
    // esp_log_level_set("wifi", ESP_LOG_NONE);

    // Create queue
    wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));

    // Start the RTOS Wifi Task
    xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL, WIFI_APP_CORE_ID);
}

BaseType_t wifi_app_send_message(wifi_app_message_e msg_id)
{
    wifi_app_queue_message_t msg;
    msg.msg_id = msg_id;

    return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

void app_main(void)
{
    // Intialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    wifi_app_start();

    configure_led();

    ESP_LOGI(TAG, "Creating wifi blink task");
    xTaskCreate(blink_wifi_task, "Blink Wifi Status Task", BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
}
