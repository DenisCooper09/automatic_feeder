#include "util/delay.h"
#include "smcspi.h"
#include "stepper_motor.h"

int main(void) {
    sm_init();
    spi_slave_init();

   // sm_run(1);

    cli();
    TCCR0A = 0x00;
    TCCR0B = 0x00;
    TCNT0 = 0xFF;
    TCCR0B |= (1 << CS00) | (1 << CS02);
    TIMSK0 |= (1 << TOIE0);
    sei();
    /*
 *
    while (1) {
        const uint8_t DATA1 = spi_slave_receive();

        if (DATA1)  sm_run(1);

        const uint8_t DATA2 = spi_slave_receive();
        if (DATA1) {
            sm_run((double) DATA2);
        }
    }*/

    while(1);
}

ISR(TIMER0_OVF_vect) {
    static volatile uint32_t counter = 0;

    if (++counter >= 67 * 10) {
        counter = 0;
        sm_init();
        sm_run(1);
    }
}
