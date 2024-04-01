// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "hal/gpio_types.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_log.h"

#include "ultrasonic.h"
#include "sg_90.h"

#define MAX_DISTANCE_M 4 // 4m max
#define MEASUREMENT_DELAY_MS 60 // 60ms

static const char TAG[] = "sonar";

uint8_t blink = 0;

ultrasonic_sensor_t sensor =
{
    .echo_pin = GPIO_NUM_17,
    .trigger_pin = GPIO_NUM_16
};

servo_motor_t motor = 
{
    .actual_angle = 0.0f,
    .channel = LEDC_CHANNEL_0,
    .timer = LEDC_TIMER_0,
    .resolution = LEDC_TIMER_15_BIT,
    .pin = GPIO_NUM_5
};

float distance = 0.0f;
float angle = 0.0f;

static void sonar_task(void *arg)
{
    for(;;)
    {
        // vTaskDelay(pdMS_TO_TICKS(MEASUREMENT_DELAY_MS * 5));
        vTaskDelay(pdMS_TO_TICKS(1000));

        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_M, &distance);
        if (res != ESP_OK)
        {
            ESP_LOGW(TAG, "Error: %X ", res);
            switch (res)
            {
                case ESP_ERR_ULTRASONIC_PING:
                    ESP_LOGW(TAG, "Cannot ping (device is in invalid state)\n");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    ESP_LOGW(TAG, "Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    ESP_LOGW(TAG, "Echo timeout (i.e. distance too big)\n");
                    break;
                default:
                    ESP_LOGW(TAG, "%s\n", esp_err_to_name(res));
            }
        }
        else
        {
            distance = distance * 100.0f; // cm
            // ESP_LOGI(TAG, "Distance: %0.04f cm\n", distance);
        }  

        gpio_set_level(GPIO_NUM_2, blink);
        // ESP_LOGI(TAG, "Blink: %d", blink);
        // ESP_LOGI(TAG, "GPIO2 Level: %d", gpio_get_level(GPIO_NUM_2));
        blink = !blink;
    }
}

static void sg_90_task(void *arg)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // ESP_LOGI(TAG, "Angle = %.2f", angle);
        ESP_ERROR_CHECK(servo_motor_set_angle(&motor, angle));

        angle += 10.0f;
        if (angle > SG_90_MAX_ANGLE)
        {
            angle = 0.0f;
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting sonar!");

    gpio_reset_pin(GPIO_NUM_14);
    gpio_config_t config = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1 << GPIO_NUM_14),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    gpio_config(&config);
    gpio_set_level(GPIO_NUM_14, 1);

    gpio_reset_pin(GPIO_NUM_2);
    config.pin_bit_mask = (1 << GPIO_NUM_2);
    gpio_config(&config);

    ultrasonic_init(&sensor);

    ESP_ERROR_CHECK(servo_motor_init(&motor));

    xTaskCreatePinnedToCore(sonar_task, "sonar_task", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(sg_90_task, "sg_90_task", 4096, NULL, 5, NULL, 0);
}
