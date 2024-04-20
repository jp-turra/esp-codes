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

// #include "nvs.h"

// Custom headear
// #include "sg_90.h"
#include "sg_90.hpp"
#include "VL53L0x.h"
#include "i2c.h"

#define SERVO_GPIO_PIN GPIO_NUM_5
#define SERVO_TIMEOUT_MS 1000

#define USE_CONTINUOUS
//#define LONG_RANGE

//#define HIGH_SPEED
#define HIGH_ACCURACY

const char *TAG = "lidar_1d";

// servo_motor_t motor;
float motor_angle = 0.0f;
float motor_step = ((SG_90_MAX_ANGLE - SG_90_MIN_ANGLE) / 10);
bool motor_increment = true;

void servo_motor_task(void *pvParameters)
{
    (void) pvParameters;
    SG_90 servo = SG_90(SERVO_GPIO_PIN, LEDC_CHANNEL_0, LEDC_TIMER_0, LEDC_TIMER_15_BIT);

    ESP_ERROR_CHECK(servo.init());

    for (;;)
    {
        // ESP_LOGI(TAG, "Angle: %f", servo._actual_angle);
        ESP_ERROR_CHECK(servo.set_angle(motor_angle));
        vTaskDelay(SERVO_TIMEOUT_MS / portTICK_PERIOD_MS);

        if (motor_angle >= SG_90_MAX_ANGLE)
        {
            motor_angle = SG_90_MAX_ANGLE;
            motor_increment = false;
        }
        else if (motor_angle <= SG_90_MIN_ANGLE)
        {
            motor_angle = SG_90_MIN_ANGLE;
            motor_increment = true;
        }

        motor_angle = motor_increment ?
            motor_angle + motor_step : motor_angle - motor_step;
    }
}

void lidar_1d_task(void *pvParameters)
{
    (void) pvParameters;

    VL53L0X sensor;

    uint8_t scl = 22;
    uint8_t sda = 21;
    i2c.init(scl, sda);
    sensor.setTimeout(500);

    if (!sensor.init()) {
        ESP_LOGE(TAG, "Failed to detect and initialize sensor!");
        while (1) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    #ifndef USE_CONTINUOUS
        #if defined LONG_RANGE
            // lower the return signal rate limit (default is 0.25 MCPS)
            sensor.setSignalRateLimit(0.1);
            // increase laser pulse periods (defaults are 14 and 10 PCLKs)
            sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
            sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        #endif

        #if defined HIGH_SPEED
            // reduce timing budget to 20 ms (default is about 33 ms)
            sensor.setMeasurementTimingBudget(20000);
        #elif defined HIGH_ACCURACY
            // increase timing budget to 200 ms
            sensor.setMeasurementTimingBudget(200000);
        #endif

        for (;;)
        {
            ESP_LOGI(TAG, "Distance: %d mm", sensor.readRangeSingleMillimeters());
            if (sensor.timeoutOccurred()) {
                ESP_LOGW(TAG, " TIMEOUT");
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    #else
        uint32_t period_ms = 60;
        sensor.startContinuous(period_ms);

        for(;;)
        {
            //TODO: Talvez tirar 40 mm de offset
            ESP_LOGI(TAG, "Distance: %d mm", sensor.readRangeSingleMillimeters());
            if (sensor.timeoutOccurred()) {
                ESP_LOGW(TAG, " TIMEOUT");
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    #endif // !USE_CONTINUOUS
    
}

extern "C"
{
    void app_main(void);
}

void app_main(void)
{
    // Initialize NVS

    xTaskCreatePinnedToCore(lidar_1d_task, "lidar_1d_task", configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(servo_motor_task, "servo_motor_task", configMINIMAL_STACK_SIZE * 2, NULL, 5, NULL, 1);
}
