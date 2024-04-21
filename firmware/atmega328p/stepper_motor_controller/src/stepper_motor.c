#include "stepper_motor.h"

static void sm_clear_timer() {
    TCNT1 = 0x0000;
}

static void sm_enable_timer() {
    SET_BIT(TCCR1B, CS10);
}

static void sm_disable_timer() {
    CLR_BIT(TCCR1B, CS10);
}

static void sm_enable_driver() {
    CLR_BIT(DRV_EN_PORT, DRV_EN_PIN);
}

static void sm_disable_driver() {
    SET_BIT(DRV_EN_PORT, DRV_EN_PIN);
}

static void sm_enable_indicator() {
    SET_BIT(LED_INDICATOR_PORT, LED_INDICATOR_PIN);
}

static void sm_disable_indicator() {
    CLR_BIT(LED_INDICATOR_PORT, LED_INDICATOR_PIN);
}

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

    /* Enable timer1 overflow interrupt. */
    SET_BIT(TIMSK1, TOIE1);

    /* Allow interrupts. */
    sei();
}

void sm_run(double revolutions) {
    revolutions_ = revolutions;

    sm_enable_driver();
    sm_enable_indicator();

    sm_clear_timer();
    sm_enable_timer();
}

void sm_resume() {
    sm_enable_driver();
    sm_enable_indicator();

    sm_enable_timer();
}

void sm_stop() {
    sm_disable_timer();

    sm_disable_driver();
    sm_disable_indicator();
}

bool sm_is_running() {
    return GET_BIT(TCCR1B, CS10);
}

ISR(TIMER1_OVF_vect) {
    static volatile uint8_t steps = 0;
    static volatile uint32_t cycle = 0;
    static volatile SM_DIRECTION dir = SM_ROTATION_DIRECTION;

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
                cycle = 0;
                dir = SM_ROTATION_DIRECTION;

                sm_stop();
            }
        }
    }

    TCNT1 = 0xFFFF - (F_CPU / SM_PULSES_FREQUENCY_HZ);
}
