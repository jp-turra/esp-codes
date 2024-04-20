// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF Libs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Custom Libs
#include "l298n.h"

l298n_t motor_driver = {
    .IN1 = GPIO_NUM_10,
    .IN2 = GPIO_NUM_11,
    .IN3 = GPIO_NUM_12,
    .IN4 = GPIO_NUM_13,
    .ENA = GPIO_NUM_20,
    .ENA_pwm = 0,
    .ENB = GPIO_NUM_21,
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
    
    for(;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Forwards");
        l298n_move(&motor_driver, L298N_LEFT_SIDE, L298N_FORWARD, 255);
        l298n_move(&motor_driver, L298N_RIGHT_SIDE, L298N_FORWARD, 255);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Backwards");
        l298n_move(&motor_driver, L298N_LEFT_SIDE, L298N_BACKWARD, 255);
        l298n_move(&motor_driver, L298N_RIGHT_SIDE, L298N_BACKWARD, 255);
    }
}

void app_main(void)
{
    xTaskCreate(motor_control_task, "motor_control", 2048, NULL, 5, NULL);
}
