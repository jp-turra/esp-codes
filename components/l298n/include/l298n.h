#ifndef L298N__H
#define L298N__H

// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// ESP-IDF Libs
#include "driver/gpio.h"
#include "driver/ledc.h"

#define L298N_PWM_TIMER_FREQ_HZ 1000    // 1kHz
#define L298N_PWM_TIMER_PERIOD  500     // 50us. 20kHz

typedef enum {
    L298N_OK = 0,
    L298N_CONFIG_PIN_ERR,
    L298N_CONFIG_PWM_ERR,
    L298N_SET_MOVE_ERR,
    L298N_ERR
} l298n_error_t;

typedef enum {
    L298N_FORWARD = 0,
    L298N_BACKWARD
} l298n_direction_t;

typedef enum {
    L298N_LEFT_SIDE = 0,
    L298N_RIGHT_SIDE,
} l298n_side_e;

typedef struct l298n {
    // GPIO Numbers
    gpio_num_t IN1;
    gpio_num_t IN2;
    gpio_num_t IN3;
    gpio_num_t IN4;
    gpio_num_t ENA;
    gpio_num_t ENB;

    // PWM Value
    uint8_t ENA_pwm;
    uint8_t ENB_pwm;

    // Direction
    l298n_direction_t ENA_direction;
    l298n_direction_t ENB_direction;

    uint32_t max_pwm_duty;
} l298n_t;

/**
 * @brief Initialize L298N pins and it's PWM
 * 
 * @param dev L298N device descriptor
 * 
 * @return 0 on success, non-zero on error
*/
l298n_error_t l298n_init(l298n_t *dev);

/**
 * @brief Activate L298N H-Bridge
 * 
 * @param dev L298N device descriptor
 * @param direction L298N direction
 * @param pwm PWM value
*/
l298n_error_t l298n_move(l298n_t *dev, l298n_side_e side, l298n_direction_t direction, uint8_t pwm);

#endif // L298N__H