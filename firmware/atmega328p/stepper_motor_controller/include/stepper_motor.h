#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#include "bits.h"
#include "ports.h"

/*

typedef enum DRV8825_STEPPER_MOTOR_MICROSTEPS {
    STEPPER_MOTOR_FULL_STEP,
    STEPPER_MOTOR_HALF_STEP,
    STEPPER_MOTOR_QUARTER_STEP,
    STEPPER_MOTOR_8_MICROSTEPS,
    STEPPER_MOTOR_16_MICROSTEPS,
    STEPPER_MOTOR_32_MICROSTEPS
} DRV8825_STEPPER_MOTOR_MICROSTEPS;

typedef enum DRV8825_STEPPER_MOTOR_DIRECTION {
    ANTICLOCKWISE,
    CLOCKWISE
} DRV8825_STEPPER_MOTOR_DIRECTION;

typedef struct DRV8825_STEPPER_MOTOR {
    volatile uint16_t rpm;
    volatile uint8_t steps_forward, steps_backward;

    volatile DRV8825_STEPPER_MOTOR_MICROSTEPS microsteps;
    volatile DRV8825_STEPPER_MOTOR_DIRECTION direction;
} DRV8825_STEPPER_MOTOR;

__attribute__((unused)) void init_stepper_motor(const DRV8825_STEPPER_MOTOR *SMC);

__attribute__((unused)) void run_stepper_motor(uint16_t revs);

__attribute__((unused)) void resume_stepper_motor(void);

__attribute__((unused)) void stop_stepper_motor(void);
*/



