// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_log.h"
#include "esp_err.h"

#include "sg_90.hpp"

static const char TAG[] = "sg_90_cpp";

/**
 * Constrain angle to be between 0 and SG_90_MAX_ANGLE
 * @param angle Pointer to angle
*/
static void constrain_angle(float *angle)
{
    if (*angle > SG_90_MAX_ANGLE || *angle < SG_90_MIN_ANGLE)
    {   
        ESP_LOGW(TAG, "Angle too big: %.2f. Constraining to max allowed value: %d", *angle, SG_90_MAX_ANGLE);
        *angle = *angle > SG_90_MAX_ANGLE ? SG_90_MAX_ANGLE : SG_90_MIN_ANGLE;
    }
    
    return;
}

SG_90::SG_90(gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer, ledc_timer_bit_t resolution)
{
    _pin = pin;
    _channel = channel;
    _timer = timer;
    _resolution = resolution;
}

esp_err_t SG_90::init()
{
    // Configure pin to be output with no interruption with pull down enabled
    RETURN_ON_ERROR(gpio_reset_pin(_pin));
    RETURN_ON_ERROR(gpio_set_direction(_pin, GPIO_MODE_OUTPUT));
    RETURN_ON_ERROR(gpio_set_intr_type(_pin, GPIO_INTR_DISABLE));
    RETURN_ON_ERROR(gpio_set_pull_mode(_pin, GPIO_PULLDOWN_ONLY));

    uint32_t max_duty = powl(2, _resolution) - 1;

    _delta_duty = (float) max_duty * (SG_90_ANGLE_180_DEG_PCT - SG_90_ANGLE_0_DEG_PCT);
    _min_duty = (float) max_duty * SG_90_ANGLE_0_DEG_PCT;
    
    // Configure ledc timer used to control servo motor
    ledc_timer_config_t ledc_timer = ledc_timer_config_t();
    ledc_timer.freq_hz = SG_90_FREQ_HZ;
    ledc_timer.duty_resolution = _resolution;
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.timer_num = _timer;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    RETURN_ON_ERROR(ledc_timer_config(&ledc_timer));

    // Configuration ledc channel used to control servo motor
    ledc_channel_config_t ledc_channel;
    ledc_channel.channel = _channel;
    ledc_channel.duty = _min_duty;
    ledc_channel.gpio_num = _pin;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.timer_sel = _timer;

    RETURN_ON_ERROR(ledc_channel_config(&ledc_channel));

    return ESP_OK;
}

esp_err_t SG_90::set_angle(float angle)
{
    // constrain_angle(&angle);

    uint32_t duty = (uint32_t) ((angle / (float) SG_90_MAX_ANGLE * _delta_duty) + _min_duty);
    
    RETURN_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, duty));
    RETURN_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel));

    _actual_angle = angle;
    return ESP_OK;
}