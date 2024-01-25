#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ports.h"

enum MOTOR_MICRO_STEPS {
    FULL_STEP __attribute__((unused)),
    MICRO_STEPS_2 __attribute__((unused)),
    MICRO_STEPS_4 __attribute__((unused)),
    MICRO_STEPS_8 __attribute__((unused)),
    MICRO_STEPS_16 __attribute__((unused)),
    MICRO_STEPS_32 __attribute__((unused))
};

enum MOTOR_DIRECTION {
    ANTI_CLOCKWISE __attribute__((unused)),
    CLOCKWISE __attribute__((unused))
};

struct stepper_motor_config {
    volatile uint16_t impulse_delay;
    volatile uint8_t forward_steps;
    volatile uint8_t backward_steps;

    volatile enum MOTOR_MICRO_STEPS micro_steps;
    volatile enum MOTOR_DIRECTION dir;
};

__attribute__((unused)) void init_stepper_motor(const struct stepper_motor_config *SMC);

__attribute__((unused)) void run_stepper_motor(uint16_t revs);

__attribute__((unused)) void resume_stepper_motor(void);

__attribute__((unused)) void stop_stepper_motor(void);
