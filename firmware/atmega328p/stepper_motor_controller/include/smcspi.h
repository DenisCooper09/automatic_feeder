#pragma once

#include <avr/io.h>

void spi_slave_init(void) {
    DDRB = (1 << PORTB4);
    SPCR = (1 << SPE);
}

uint8_t spi_slave_receive(void) {
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}
