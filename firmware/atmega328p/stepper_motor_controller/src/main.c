#include "smcspi.h"
#include "stepper_motor.h"

int main(void) {
    sm_init();
    spi_slave_init();

    while (1) {
        const uint8_t COMMAND = spi_slave_receive();

        if (COMMAND == 0x1F && !sm_is_running())  sm_run(spi_slave_receive());
    }
}
