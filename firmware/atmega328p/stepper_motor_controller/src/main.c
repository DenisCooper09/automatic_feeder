#include "smcspi.h"
#include "stepper_motor.h"
#include "stepper_motor_config.h"

int main(void) {
    spi_slave_init();

    struct stepper_motor_config smc;
    smc.impulse_delay = SM_IMPULSE_DELAY;
    smc.forward_steps = SM_FORWARD_STEPS;
    smc.backward_steps = SM_BACKWARD_STEPS;
    smc.micro_steps = SM_MICRO_STEPS;
    smc.dir = SM_DIRECTION;
    init_stepper_motor(&smc);

    while (1) {
        const uint8_t DATA1 = spi_slave_receive();
        const uint8_t DATA2 = spi_slave_receive();
        if (DATA1) {
            run_stepper_motor(DATA2);
        }
    }

    return 0;
}
