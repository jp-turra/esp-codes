// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF Libs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_console.h"

// Custom Libs
#include "l298n.h"

l298n_t motor_driver = {
    .IN1 = GPIO_NUM_32,
    .IN3 = GPIO_NUM_33,
    .ENA = GPIO_NUM_25,
    .ENA_pwm = 0,
    .IN2 = GPIO_NUM_NC,
    .IN4 = GPIO_NUM_NC,
    .ENB = GPIO_NUM_NC,
    .ENB_pwm = 0,
    .ENA_direction = L298N_FORWARD,
    .ENB_direction = L298N_FORWARD,
    .max_pwm_duty = 0,
};

static const char *TAG = "motor_control";

void motor_control_task(void *pvParameters)
{
    (void) pvParameters;

    l298n_error_t err = l298n_init(&motor_driver);

    if (err != L298N_OK)
    {
        ESP_LOGE(TAG, "l298n_init failed: %d", err);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        return;
    }

    int16_t pwm = 255;
    
    for(;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Forwards");
        l298n_move(&motor_driver, L298N_LEFT_SIDE, L298N_FORWARD, pwm);
        // l298n_move(&motor_driver, L298N_RIGHT_SIDE, L298N_FORWARD, pwm);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "STOP");
        l298n_move(&motor_driver, L298N_LEFT_SIDE, L298N_FORWARD, 0);
        // l298n_move(&motor_driver, L298N_RIGHT_SIDE, L298N_FORWARD, 0);
    }
}

void set_pwm(float pwm)
{
    l298n_move(&motor_driver, L298N_LEFT_SIDE, pwm > 0 ? L298N_FORWARD : L298N_BACKWARD, pwm);
}

void app_main(void)
{
    xTaskCreate(motor_control_task, "motor_control", 2048, NULL, 5, NULL);
}
