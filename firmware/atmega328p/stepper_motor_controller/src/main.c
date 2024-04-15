#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include "bits.h"

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

///////////////////////////////////////////////////////
// Stepper Motor
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

int main(void) {
    /*
    spi_slave_init();

    DRV8825_STEPPER_MOTOR smc;
    smc.rpm = SM_RPM;
    smc.steps_forward = SM_FORWARD_STEPS;
    smc.steps_backward = SM_BACKWARD_STEPS;
    smc.microsteps = SM_MICRO_STEPS;
    smc.direction = SM_DIRECTION;
    init_stepper_motor(&smc);

    while (1) {
        const uint8_t DATA1 = spi_slave_receive();
        const uint8_t DATA2 = spi_slave_receive();
        if (DATA1) {
            run_stepper_motor(DATA2);
        }
    }

    return 0;
*/


    /* Set all pins for controlling the driver as outputs. */
    SET_BIT(DRV_M0_DDR, DRV_M0_PIN);
    SET_BIT(DRV_M1_DDR, DRV_M1_PIN);
    SET_BIT(DRV_M2_DDR, DRV_M2_PIN);
    SET_BIT(DRV_EN_DDR, DRV_EN_PIN);
    SET_BIT(DRV_DIR_DDR, DRV_DIR_PIN);
    SET_BIT(DRV_STEP_DDR, DRV_STEP_PIN);

    /* Configure microstepping. */
    SET_BIT(DRV_M0_PORT, DRV_M0_PIN);
    SET_BIT(DRV_M1_PORT, DRV_M1_PIN);
    SET_BIT(DRV_M2_PORT, DRV_M2_PIN);

    /* Enable the driver. */
    CLR_BIT(DRV_EN_PORT, DRV_EN_PIN);

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

    /* Enable Timer1 overflow interrupt. */
    SET_BIT(TIMSK1, TOIE1);

    /* Allow interrupts. */
    sei();

    while (1);
}

#define SM_FREQUENCY_HZ 40000

#define SM_STEPS_FORWARD 100
#define SM_STEPS_BACKWARD 90

#define SM_REVOLUTIONS 0.5

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
            if (++cycle >= (uint32_t) ((200.0 * 32) / (SM_STEPS_FORWARD - SM_STEPS_BACKWARD) * SM_REVOLUTIONS)) {
                CLR_BIT(TIMSK1, TOIE1);
                TCNT1 = 0x0000;
            }
        }
    }

    TCNT1 = 0xFFFF - (F_CPU / SM_FREQUENCY_HZ);
}
