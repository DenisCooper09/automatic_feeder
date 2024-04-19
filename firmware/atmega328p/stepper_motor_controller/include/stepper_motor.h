#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>

#include "bits.h"

///////////////////////////////////////////////////////
// Ports
///////////////////////////////////////////////////////

#define DRV_EN_DDR DDRB
#define DRV_EN_PIN PORTB1
#define DRV_EN_PORT PORTB

#define DRV_M0_DDR DDRB
#define DRV_M0_PIN PORTB0
#define DRV_M0_PORT PORTB

#define DRV_M1_DDR DDRD
#define DRV_M1_PIN PORTD7
#define DRV_M1_PORT PORTD

#define DRV_M2_DDR DDRD
#define DRV_M2_PIN PORTD6
#define DRV_M2_PORT PORTD

#define DRV_DIR_DDR DDRD
#define DRV_DIR_PIN PORTD4
#define DRV_DIR_PORT PORTD

#define DRV_STEP_DDR DDRD
#define DRV_STEP_PIN PORTD5
#define DRV_STEP_PORT PORTD

#define LED_INDICATOR_DDR DDRD
#define LED_INDICATOR_PIN PORTD2
#define LED_INDICATOR_PORT PORTD

///////////////////////////////////////////////////////
// Settings
///////////////////////////////////////////////////////

#define SM_PULSES_FREQUENCY_HZ 40000

#define SM_STEPS_FORWARD 100
#define SM_STEPS_BACKWARD 90

///////////////////////////////////////////////////////
// Enums
///////////////////////////////////////////////////////

typedef enum SM_STEP_MODE {
    SM_FULL_STEP,
    SM_HALF_STEP,
    SM_QUARTER_STEP,
    SM_8_MICROSTEPS,
    SM_16_MICROSTEPS,
    SM_32_MICROSTEPS
} SM_STEP_MODE;

typedef enum SM_DIRECTION {
    ANTICLOCKWISE,
    CLOCKWISE
} SM_DIRECTION;

///////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////

static volatile double revolutions_;
static bool is_running_;

///////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////

void sm_init();

void sm_run(double revolutions);

void sm_stop();

bool sm_is_running();

void sm_enable_drv();

void sm_disable_drv();

void sm_enable_tmr();

void sm_disable_tmr();

void sm_enable_indicator();

void sm_disable_indicator();
