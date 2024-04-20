// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF Libs
#include "freertos/FreeRTOS.h"
#include "driver/ledc.h"

#include "esp_log.h"
#include "esp_err.h"

// Custom Libs
#include "l298n.h"

#define RETURN_ON_ERROR(err, ret_code) if (err != ESP_OK) { return ret_code; }

// static const char *TAG = "l298n";

l298n_error_t configure_output_pin(gpio_num_t pin, bool configure_pwm, ledc_timer_t timer_num, ledc_channel_t channel_num, uint32_t *max_pwm_duty)
{
    esp_err_t err;
    RETURN_ON_ERROR(gpio_reset_pin(pin), L298N_CONFIG_PIN_ERR);
    RETURN_ON_ERROR(gpio_set_direction(pin, GPIO_MODE_OUTPUT), L298N_CONFIG_PIN_ERR);
    RETURN_ON_ERROR(gpio_set_intr_type(pin, GPIO_INTR_DISABLE), L298N_CONFIG_PIN_ERR);
    RETURN_ON_ERROR(gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY), L298N_CONFIG_PIN_ERR);
    RETURN_ON_ERROR(gpio_set_level(pin, 0), L298N_CONFIG_PIN_ERR);

    if (configure_pwm)
    {   
        *max_pwm_duty = ledc_find_suitable_duty_resolution(LEDC_REF_TICK, L298N_PWM_TIMER_FREQ_HZ);
        if (*max_pwm_duty == 0)
        {
            return L298N_CONFIG_PWM_ERR;
        }
        // Configuring timer
        ledc_timer_config_t ledc_timer = {
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_num = timer_num,
            .freq_hz = L298N_PWM_TIMER_FREQ_HZ,
            .duty_resolution = *max_pwm_duty,
            .clk_cfg = LEDC_REF_TICK
        };
        RETURN_ON_ERROR(ledc_timer_config(&ledc_timer), L298N_CONFIG_PWM_ERR);

        // Configuring channel
        ledc_channel_config_t ledc_channel = {
            .channel = channel_num,
            .gpio_num = pin,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_sel = timer_num,
            .duty = 0
        };
        RETURN_ON_ERROR(ledc_channel_config(&ledc_channel), L298N_CONFIG_PWM_ERR);
    }

    return L298N_OK;
}

l298n_error_t l298n_init(l298n_t *dev)
{
    RETURN_ON_ERROR(-1, configure_output_pin(dev->IN1, false, 0, 0, NULL));
    RETURN_ON_ERROR(-1, configure_output_pin(dev->IN2, false, 0, 0, NULL));
    RETURN_ON_ERROR(-1, configure_output_pin(dev->IN3, false, 0, 0, NULL));
    RETURN_ON_ERROR(-1, configure_output_pin(dev->IN4, false, 0, 0, NULL));

    RETURN_ON_ERROR(-1, configure_output_pin(dev->ENA, true, LEDC_TIMER_0, LEDC_CHANNEL_0, &dev->max_pwm_duty));
    RETURN_ON_ERROR(-1, configure_output_pin(dev->ENB, true, LEDC_TIMER_1, LEDC_CHANNEL_1, &dev->max_pwm_duty));

    return L298N_OK;
}

l298n_error_t l298n_move(l298n_t *dev, l298n_side_e side, l298n_direction_t direction, uint8_t pwm)
{
    gpio_num_t ina_pin = side == L298N_LEFT_SIDE ? dev->IN1 : dev->IN2;
    gpio_num_t inb_pin = side == L298N_LEFT_SIDE ? dev->IN3 : dev->IN4;

    uint32_t duty = (uint32_t) (((float) pwm / 255.0) * dev->max_pwm_duty);
    if (direction == L298N_FORWARD)
    {
        RETURN_ON_ERROR(gpio_set_level(ina_pin, 1), L298N_SET_MOVE_ERR);
        RETURN_ON_ERROR(gpio_set_level(inb_pin, 0), L298N_SET_MOVE_ERR);
    }
    else
    {
        RETURN_ON_ERROR(gpio_set_level(ina_pin, 0), L298N_SET_MOVE_ERR);
        RETURN_ON_ERROR(gpio_set_level(inb_pin, 1), L298N_SET_MOVE_ERR);
    }

    RETURN_ON_ERROR(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty), L298N_SET_MOVE_ERR);
    RETURN_ON_ERROR(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty), L298N_SET_MOVE_ERR);

    RETURN_ON_ERROR(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0), L298N_SET_MOVE_ERR);
    RETURN_ON_ERROR(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1), L298N_SET_MOVE_ERR);
}
