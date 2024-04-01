#ifndef SERVO_CONTROL_SG_90__H
#define SERVO_CONTROL_SG_90__H

// Standart Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_err.h"

#define SG_90_MAX_ANGLE 180
#define SG_90_FREQ_HZ 50
// #define SG_90_FREQ_MS 0.002

// #define SG_90_0_DEG_TIME_MS 1
// #define SG_90_180_DEG_TIME_MS 2

#define SG_90_ANGLE_0_DEG_PCT 0.05f
#define SG_90_ANGLE_180_DEG_PCT 0.1f

typedef struct servo_motor {
    gpio_num_t pin;
    float actual_angle; // 0 - 180 º

    ledc_channel_t channel;
    ledc_timer_t timer;
    ledc_timer_bit_t resolution;
} servo_motor_t;


/**
 * Initialize Servo Motor
 * @param servo_motor Servo motor pointer
*/
esp_err_t servo_motor_init(servo_motor_t *servo_motor);

/**
 * Set Servo Motor Angle
 * @param servo_motor Servo motor pointer
 * @param angle Servo motor angle
*/
esp_err_t servo_motor_set_angle(servo_motor_t *servo_motor, float angle);

#endif // !SERVO_CONTROL_SG_90__H