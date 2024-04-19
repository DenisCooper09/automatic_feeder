#include "stepper_motor.h"

void sm_init() {
    /* Set all control pins as outputs. */
    SET_BIT(DRV_M0_DDR, DRV_M0_PIN);
    SET_BIT(DRV_M1_DDR, DRV_M1_PIN);
    SET_BIT(DRV_M2_DDR, DRV_M2_PIN);
    SET_BIT(DRV_EN_DDR, DRV_EN_PIN);
    SET_BIT(DRV_DIR_DDR, DRV_DIR_PIN);
    SET_BIT(DRV_STEP_DDR, DRV_STEP_PIN);
    SET_BIT(LED_INDICATOR_DDR, LED_INDICATOR_PIN);

    /* Disable the driver. */
    SET_BIT(DRV_EN_PORT, DRV_EN_PIN);

    /* Configure microstepping (32 microsteps/step). */
    SET_BIT(DRV_M0_PORT, DRV_M0_PIN);
    SET_BIT(DRV_M1_PORT, DRV_M1_PIN);
    SET_BIT(DRV_M2_PORT, DRV_M2_PIN);

    /* Set motor direction to clockwise. */
    SET_BIT(DRV_DIR_PORT, DRV_DIR_PIN);

    CLR_BIT(DRV_STEP_PORT, DRV_STEP_PIN);

    /* Disable interrupts. */
    cli();

    /* Clear timer registers. */
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1 = 0x0000;

    /* Set prescaler to 1 (no prescaling). */
    SET_BIT(TCCR1B, CS10);

    /* Allow interrupts. */
    sei();
}

void sm_run(double revolutions) {
    revolutions_ = revolutions;

    sm_enable_drv();
    sm_enable_indicator();

    SET_BIT(TIMSK1, TOIE1);
}

void sm_enable_drv() {
    CLR_BIT(DRV_EN_PORT, DRV_EN_PIN);
}

void sm_disable_drv() {
    SET_BIT(DRV_EN_PORT, DRV_EN_PIN);
}

void sm_enable_indicator() {
    SET_BIT(LED_INDICATOR_PORT, LED_INDICATOR_PIN);
}

void sm_disable_indicator() {
    CLR_BIT(LED_INDICATOR_PORT, LED_INDICATOR_PIN);
}

ISR(TIMER1_OVF_vect) {
    static volatile uint8_t steps = 0;
    static volatile uint32_t cycle = 0;
    static volatile SM_DIRECTION dir = CLOCKWISE;

    INV_BIT(DRV_STEP_PORT, DRV_STEP_PIN);

    if (GET_BIT(DRV_STEP_PORT, DRV_STEP_PIN)) {
        steps++;

        if (dir == CLOCKWISE && steps >= SM_STEPS_FORWARD) {
            INV_BIT(DRV_DIR_PORT, DRV_DIR_PIN);
            steps = 0;
            dir = ANTICLOCKWISE;
        }

        if (dir == ANTICLOCKWISE && steps >= SM_STEPS_BACKWARD) {
            INV_BIT(DRV_DIR_PORT, DRV_DIR_PIN);
            steps = 0;
            dir = CLOCKWISE;

            /*
             * Full revolution takes 200 * 32 = 6400 microsteps.
             *
             * One cycle is 100 steps forward and 90 steps backward.
             * So in total motor has made 10 steps forward.
             *
             * Dividing total number of microsteps (6400) by steps forward gives number of cycles to make one revolution.
             *
             * 6400 / (100 - 90) = 640 cycles
             *
             * By multiplying number of cycles by a constant ("SM_REVOLUTIONS") we can control number of revolutions we want motor to make.
             * */
            if (++cycle >= (uint32_t) ((200.0 * 32) / (SM_STEPS_FORWARD - SM_STEPS_BACKWARD) * revolutions_)) {
                CLR_BIT(TIMSK1, TOIE1);
                TCNT1 = 0x0000;

                sm_disable_drv();
                sm_disable_indicator();
            }
        }
    }

    TCNT1 = 0xFFFF - (F_CPU / SM_PULSES_FREQUENCY_HZ);
}
