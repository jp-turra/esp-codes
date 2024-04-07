// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "esp_log.h"

#include "sg_90.h"

#define RETURN_ON_ERROR(x) if ((x) != ESP_OK) { return x; }

static const char TAG[] = "sg_90";

static float min_duty = 0;
static float delta_duty = 0;

/**
 * Constrain angle to be between 0 and SG_90_MAX_ANGLE
 * @param angle Pointer to angle
*/
static void constrain_angle(float *angle)
{
    if (*angle > SG_90_MAX_ANGLE || *angle < SG_90_MIN_ANGLE * 2)
    {   
        ESP_LOGW(TAG, "Angle too big: %.2f. Constraining to max allowed value: %d", *angle, SG_90_MAX_ANGLE);
        *angle = *angle > SG_90_MAX_ANGLE ? SG_90_MAX_ANGLE : SG_90_MIN_ANGLE;
    }
    
    return;
}

esp_err_t servo_motor_init(servo_motor_t *servo_motor)
{
    // Configure pin to be output with no interruption with pull down enabled
    RETURN_ON_ERROR(gpio_reset_pin(servo_motor->pin));
    RETURN_ON_ERROR(gpio_set_direction(servo_motor->pin, GPIO_MODE_OUTPUT));
    RETURN_ON_ERROR(gpio_set_intr_type(servo_motor->pin, GPIO_INTR_DISABLE));
    RETURN_ON_ERROR(gpio_set_pull_mode(servo_motor->pin, GPIO_PULLDOWN_ONLY));

    uint32_t max_duty = pow(2, servo_motor->resolution) - 1;

    delta_duty = (float) max_duty * (SG_90_ANGLE_180_DEG_PCT - SG_90_ANGLE_0_DEG_PCT);
    min_duty = (float) max_duty * SG_90_ANGLE_0_DEG_PCT;
    
    // Configure ledc timer used to control servo motor
    ledc_timer_config_t ledc_timer = {
        .freq_hz = SG_90_FREQ_HZ,
        .duty_resolution = servo_motor->resolution,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = servo_motor->timer,
    };
    RETURN_ON_ERROR(ledc_timer_config(&ledc_timer));

    // Configuration ledc channel used to control servo motor
    ledc_channel_config_t ledc_channel = {
        .channel = servo_motor->channel,
        .duty = min_duty,
        .gpio_num = servo_motor->pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = servo_motor->timer
    };
    RETURN_ON_ERROR(ledc_channel_config(&ledc_channel));

    return ESP_OK;
}

esp_err_t servo_motor_set_angle(servo_motor_t *servo_motor, float angle)
{
    constrain_angle(&angle);

    uint32_t duty = (uint32_t) ((angle / (float) SG_90_MAX_ANGLE * delta_duty) + min_duty);
    
    // ESP_LOGI(TAG, "Angle: %.2f, Duty: %li", angle, duty);

    // RETURN_ON_ERROR(ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, servo_motor->channel, duty, -1));
    RETURN_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, servo_motor->channel, duty));
    RETURN_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, servo_motor->channel));

    servo_motor->actual_angle = angle;
    return ESP_OK;
}
