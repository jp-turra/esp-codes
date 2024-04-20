// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "nvs.h"

// Custom headear
#include "sg_90.h"
#include "vl53l0x.h"
#include "ST_API/platform/inc/vl53l0x_platform.h"

#define SERVO_GPIO_PIN GPIO_NUM_5;

const char *TAG = "lidar_1d";

servo_motor_t motor;
float motor_angle = 0.0f;
float motor_step = (SG_90_MAX_ANGLE / 10);
bool motor_increment = true;

void servo_motor_task(void *pvParameters)
{
    (void) pvParameters;

    motor.actual_angle = 0;
    motor.channel = LEDC_CHANNEL_0;
    motor.timer = LEDC_TIMER_0;
    motor.resolution = LEDC_TIMER_15_BIT;
    motor.pin = SERVO_GPIO_PIN;

    ESP_ERROR_CHECK(servo_motor_init(&motor));

    for (;;)
    {
        ESP_LOGI(TAG, "Angle: %f", motor.actual_angle);
        servo_motor_set_angle(&motor, motor_angle);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if (motor_angle >= SG_90_MAX_ANGLE)
        {
            motor_angle = SG_90_MAX_ANGLE;
            motor_increment = false;
        }
        else if (motor_angle <= 0)
        {
            motor_angle = 0;
            motor_increment = true;
        }

        motor_angle = motor_increment ?
            motor_angle + motor_step : motor_angle - motor_step;
    }
}

void lidar_1d_task(void *pvParameters)
{
    (void) pvParameters;

    VL53L0X_Error err = vl53l0x_start(uint8_t pin, bool calibration_mask, VL53L0X_DeviceModes dev_mode, nvs_handle_t nvs_handle, 
    uint32_t offset_distance_mm, uint32_t x_talk_distance);
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    nvs_handle_t my_handle;
    err = nvs_open("vl53l0x", NVS_READWRITE, &my_handle);
    ESP_ERROR_CHECK( err );

    xTaskCreatePinnedToCore(servo_motor_task, "servo_motor_task", configMINIMAL_STACK_SIZE * 2, NULL, 5, NULL, 0);    

    nvs_close(my_handle);
}
