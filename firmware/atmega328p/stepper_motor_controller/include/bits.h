#ifndef STEPPER_MOTOR_CONTROLLER_BITS_H
#define STEPPER_MOTOR_CONTROLLER_BITS_H

#define SET_BIT(reg, ...) \
    do { \
        uint8_t bits[] = {__VA_ARGS__}; \
        for (uint32_t bit = 0; bit < sizeof(bits) / sizeof(bits[0]); ++bit) { \
            (reg) |= (1 << (bits[bit])); \
        } \
    } while(0)

#define VAL_BIT(reg, val, ...) \
    do { \
        uint8_t bits[] = {__VA_ARGS__}; \
        for (uint32_t bit = 0; bit < sizeof(bits) / sizeof(bits[0]); ++bit) { \
            (reg) |= ((1 << (bits[bit])) & (val << (bits[bit]))); \
        } \
    } while(0)

#define CLR_BIT(reg, ...) \
    do { \
        uint8_t bits[] = {__VA_ARGS__}; \
        for (uint32_t bit = 0; bit < sizeof(bits) / sizeof(bits[0]); ++bit) { \
            (reg) &= ~(1 << (bits[bit])); \
        } \
    } while(0)

#define INV_BIT(reg, ...) \
    do { \
        uint8_t bits[] = {__VA_ARGS__}; \
        for (uint32_t bit = 0; bit < sizeof(bits) / sizeof(bits[0]); ++bit) { \
            (reg) ^= (1 << (bits[bit])); \
        } \
    } while(0)

#define GET_BIT(reg, bit) ((reg) & (1 << (bit)))

#endif /* STEPPER_MOTOR_CONTROLLER_BITS_H */
