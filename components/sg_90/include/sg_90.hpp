#if !defined(SG_90__HP)
#define SG_90__HP

// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_err.h"

#define SG_90_MAX_ANGLE 180
#define SG_90_MIN_ANGLE (SG_90_MAX_ANGLE - 180)
#define SG_90_FREQ_HZ 50

#define SG_90_ANGLE_0_DEG_PCT 0.03f // 0.05 in datasheet
#define SG_90_ANGLE_180_DEG_PCT 0.12f // 0.1 in datasheet

#define RETURN_ON_ERROR(x) if ((x) != ESP_OK) { return x; }

class SG_90
{
    
public:
    SG_90(gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer, ledc_timer_bit_t resolution);
    ~SG_90() = default;

    esp_err_t init();
    esp_err_t set_angle(float angle);

    gpio_num_t _pin;
    float _actual_angle; // 0 - 180 º

    ledc_channel_t _channel;
    ledc_timer_t _timer;
    ledc_timer_bit_t _resolution;

    float _delta_duty;
    float _min_duty;
};


#endif // SG_90__HP
