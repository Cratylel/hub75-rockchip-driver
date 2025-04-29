#ifndef _GPIO_H
#define _GPIO_H

#include "config.h"
#include "log.h"
#include <stdlib.h>
#include <inttypes.h>
#include <cstdint>
extern volatile uint32_t *gpio_regs[5];
namespace GPIO
{
    static inline void set_bit(uint32_t *var, unsigned int bit_num, unsigned int value)
    {
        if (bit_num >= 32)
            return; // Ensure bit number is within 0-31 range
        uint32_t mask = (uint32_t)1 << bit_num;
        if (value)
        {
            *var |= mask; // Set the bit
        }
        else
        {
            *var &= ~mask; // Clear the bit
        }
    }

    // static inline void write_to_data_register(uint8_t gpio_chip, uint8_t low_high, uint32_t data);
    //static inline void write_to_direction_register(uint8_t gpio_chip, uint8_t low_high, uint32_t data);

    void set_pin_mode(uint8_t pin, bool direction);
    inline void set_pin(uint8_t pin, bool value);

    void set_pin_map(config::pin_mapping new_pin_map);

    int init();
    int exit();
    static inline void write_gpio_value(volatile uint32_t *registers, uint32_t offset, uint32_t value)
    {
        // Directly assign the value to the register
#ifndef TEST_ENV
        registers[offset / 4] = value;
#endif

// Synchronize the write operation (ensure it's propagated)
#ifndef TEST_ENV
        // asm volatile("dmb" ::: "memory"); // Data memory barrier for ARM
#endif
    }

    inline void write_to_data_register(uint8_t gpio_chip, uint8_t low_high, uint32_t data)
    {
        if (low_high == 0)
        {
            write_gpio_value(gpio_regs[gpio_chip], 0x0000, data);
        }
        else
        {
            write_gpio_value(gpio_regs[gpio_chip], 0x0004, data);
        }
    }

    inline void set_pin(uint8_t pin, bool value)
    {

        if (pin > 159)
        {
            LOG_ERROR("Pin number out of range: %d", pin);
            return;
        }

        // Determine the GPIO chip (each chip has 32 pins)
        int gpio_chip_num = pin / 32;

        // Determine whether it's the low or high register
        int pin_within_chip = pin % 32;
        int register_type = (pin_within_chip < 16) ? 0 : 1;

        // Pin within the register (0-15 for low, 16-31 for high)
        int pin_in_register = pin_within_chip % 16;
        uint32_t reg_data = 0;
        GPIO::set_bit(&reg_data, pin_in_register + 16, 1); // set write mask
        GPIO::set_bit(&reg_data, pin_in_register, value);  // set actual pin bit
#ifdef TEST_ENV
#ifdef GPIO_DEBUG_LOG
        LOG_DEBUG("Setting pin value: chipnum: %d, pin_within_chip: %02d, register_type: %d, pin_in_register: %02d, pin_value: %d, reg_data: %X", gpio_chip_num, pin_within_chip, register_type, pin_in_register, value, reg_data);
#endif
#endif
        write_to_data_register(gpio_chip_num, register_type, reg_data);
    }
} // namespace GPIO

#endif