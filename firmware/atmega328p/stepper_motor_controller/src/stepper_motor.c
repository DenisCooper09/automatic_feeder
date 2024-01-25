#include "stepper_motor.h"

static volatile struct stepper_motor_config smc;

static volatile uint16_t stepper_motor_revs;

__attribute__((unused)) void init_stepper_motor(const struct stepper_motor_config *SMC) {
    smc = *SMC;

    LED_DDR |= (1 << LED_PIN);
    LED_PORT &= ~(1 << LED_PIN);

    DRV_M0_DDR |= (1 << DRV_M0_PIN);
    DRV_M1_DDR |= (1 << DRV_M1_PIN);
    DRV_M2_DDR |= (1 << DRV_M2_PIN);

    DRV_M0_PORT |= ((1 & (uint8_t) smc.micro_steps) << DRV_M0_PIN);
    DRV_M1_PORT |= ((1 & (((uint8_t) smc.micro_steps & (1 << 1)) >> 1)) << DRV_M1_PIN);
    DRV_M2_PORT |= ((1 & (((uint8_t) smc.micro_steps & (1 << 2)) >> 2)) << DRV_M2_PIN);

    DRV_EN_DDR |= (1 << DRV_EN_PIN);
    DRV_EN_PORT |= (1 << DRV_EN_PIN);

    DRV_DIR_DDR |= (1 << DRV_DIR_PIN);
    DRV_DIR_PORT |= ((1 & (uint8_t) smc.dir) << DRV_DIR_PIN);

    DRV_STEP_DDR |= (1 << DRV_STEP_PIN);
    DRV_STEP_PORT &= ~(1 << DRV_STEP_PIN);

    cli();
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12) | (1 << CS10);
    TCNT1 = 0;
    OCR1A = (F_CPU * smc.impulse_delay) / 1000000UL - 1;
    sei();
}

__attribute__((unused)) void run_stepper_motor(uint16_t revs) {
    stepper_motor_revs = revs;
    DRV_EN_PORT &= ~(1 << DRV_EN_PIN);
    LED_PORT |= (1 << LED_PIN);
    asm volatile("nop");
    TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1A);
}

__attribute__((unused)) void resume_stepper_motor(void) {
    DRV_EN_PORT &= ~(1 << DRV_EN_PIN);
    LED_PORT |= (1 << LED_PIN);
    asm volatile("nop");
    TIMSK1 |= (1 << OCIE1A);
}

__attribute__((unused)) void stop_stepper_motor(void) {
    TIMSK1 &= ~(1 << OCIE1A);
    asm volatile("nop");
    DRV_EN_PORT |= (1 << DRV_EN_PIN);
    LED_PORT &= ~(1 << LED_PIN);
}

ISR(TIMER1_COMPA_vect) {
    static volatile uint8_t dir = 1;
    static volatile uint32_t step = 0;
    static volatile uint32_t revs = 0;

    DRV_STEP_PORT ^= (1 << DRV_STEP_PIN);
    if (DRV_STEP_PORT & (1 << DRV_STEP_PIN)) {
        if (dir && step >= smc.forward_steps) {
            DRV_DIR_PORT ^= (1 << DRV_DIR_PIN);
            step = 0;
            dir = 0;

        } else if (!dir && step >= smc.backward_steps) {
            DRV_DIR_PORT ^= (1 << DRV_DIR_PIN);
            step = 0;
            dir = 1;

            const volatile uint16_t REQUIRED_STEPS = 200 * (1 << (uint8_t) smc.micro_steps);
            if (++revs >= (REQUIRED_STEPS / (smc.forward_steps - smc.backward_steps) * stepper_motor_revs)) {
                revs = 0;
                stop_stepper_motor();
                TCNT1 = 0;
            }
        } else step++;
    }
}
