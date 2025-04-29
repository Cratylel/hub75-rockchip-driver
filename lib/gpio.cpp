#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include "gpio.h"
#include "config.h"
// #include <execinfo.h>
#include <iostream>
#include "log.h"

#define STANDARD_GPIO_SIZE 0x1000

#define GPIO0_BASE 0xFF380000
#define GPIO0_SIZE STANDARD_GPIO_SIZE
#define GPIO1_BASE 0xFF530000
#define GPIO1_SIZE STANDARD_GPIO_SIZE
#define GPIO2_BASE 0xFF540000
#define GPIO2_SIZE STANDARD_GPIO_SIZE
#define GPIO3_BASE 0xFF550000
#define GPIO3_SIZE STANDARD_GPIO_SIZE
#define GPIO4_BASE 0xFFF56000
#define GPIO4_SIZE STANDARD_GPIO_SIZE

#define GPIO_SWPORT_DR_L 0x0000
#define GPIO_SWPORT_DR_H 0x0004
#define GPIO_SWPORT_DDR_L 0x0008
#define GPIO_SWPORT_DDR_H 0x000C
#define GPIO_CHIP_SIZE 8192
#define NUM_GPIO_CHIPS 5
#define OUTPUT 1
#define INPUT 0

// init registers array
volatile uint32_t *gpio_regs[NUM_GPIO_CHIPS] = {0};

// config::pin_mapping pinmap;

//*register mapping

#ifdef TEST_ENV
// mem alloc for testing
void init_gpio_regs_testenv()
{
    LOG_DEBUG("Allocating test memory for GPIO registers");
    for (int i = 0; i < NUM_GPIO_CHIPS; ++i)
    {
        gpio_regs[i] = (volatile uint32_t *)calloc(GPIO_CHIP_SIZE, sizeof(uint32_t));
        if (!gpio_regs[i])
        {
            fprintf(stderr, "Failed to allocate test memory for gpio_regs[%d]\n", i);
            exit(1);
        }
    }
}
#endif

// map registers of base address and size, returns registers
volatile uint32_t *map_registers(uint32_t base_addr, uint32_t size)
{
    LOG_DEBUG("Mapping registers 0x%X with size 0x%X", base_addr, size);
#ifdef TEST_ENV
    // LOG_DEBUG("Mapping registers 0x%X with size 0x%X", base_addr, size);
    return 0;
#else
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1)
    {
        perror("Error opening /dev/mem");
        exit(1);
    }
    volatile uint32_t *_gpio_regs = (volatile uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, base_addr);
    if (_gpio_regs == MAP_FAILED)
    {
        LOG_ERROR("Error mapping register 0x%X with size 0x%X", base_addr, size);

        close(mem_fd);
        exit(2);
    }
    else
    {
        return _gpio_regs;
    }
#endif
}

// unmap registers of size
void unmap_registers(volatile uint32_t *registers, uint32_t size)
{
    LOG_DEBUG("Unmapping register");
#ifndef TEST_ENV
    if (registers != MAP_FAILED)
    {
        munmap((void *)registers, size);
    }
#endif
}

//*register writing

// Function to write a value to a specific register at a given offset (4 byte aligned)
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

// Function to check the value of a specific register at a given offset
//* void check_gpio_value(int offset)
// {
//     volatile uint32_t *reg = gpio1_regs[offset / 4];
//     uint32_t reg_value = *reg;
//     printf("Register value at offset 0x%X: 0x%X\n", offset, reg_value);
// }

#define LOW 0
#define HIGH 1

// static inline void GPIO::write_to_data_register(uint8_t gpio_chip, uint8_t low_high, uint32_t data)
// {
//     if (low_high == 0)
//     {
//         write_gpio_value(gpio_regs[gpio_chip], GPIO_SWPORT_DR_L, data);
//     }
//     else
//     {
//         write_gpio_value(gpio_regs[gpio_chip], GPIO_SWPORT_DR_H, data);
//     }
// }

static inline void write_to_direction_register(uint8_t gpio_chip, uint8_t low_high, uint32_t data)
{
    if (low_high == 0)
    {
        write_gpio_value(gpio_regs[gpio_chip], GPIO_SWPORT_DDR_L, data);
    }
    else
    {
        write_gpio_value(gpio_regs[gpio_chip], GPIO_SWPORT_DDR_H, data);
    }
}

void GPIO::set_pin_mode(uint8_t pin, bool direction)
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
    int register_type = (pin_within_chip < 16) ? LOW : HIGH;

    // Pin within the register (0-15 for low, 16-31 for high)
    int pin_in_register = pin_within_chip % 16;
    uint32_t reg_data = 0;

    GPIO::set_bit(&reg_data, pin_in_register + 16, 1);    // set write mask
    GPIO::set_bit(&reg_data, pin_in_register, direction); // set actual direction bit
#ifdef TEST_ENV
#ifdef GPIO_DEBUG_LOG
    LOG_DEBUG("Setting pin direction: chipnum: %d, pin_within_chip: %d, register_type: %d, pin_in_register: %d, reg_data: %X", gpio_chip_num, pin_within_chip, register_type, pin_in_register, reg_data);
#endif
#endif
    write_to_direction_register(gpio_chip_num, register_type, reg_data);
}

// inline void GPIO::set_pin(uint8_t pin, bool value)
// {
//     if (pin > 159)
//     {
//         LOG_ERROR("Pin number out of range: %d", pin);
//         return;
//     }

//     // Determine the GPIO chip (each chip has 32 pins)
//     int gpio_chip_num = pin / 32;

//     // Determine whether it's the low or high register
//     int pin_within_chip = pin % 32;
//     int register_type = (pin_within_chip < 16) ? LOW : HIGH;

//     // Pin within the register (0-15 for low, 16-31 for high)
//     int pin_in_register = pin_within_chip % 16;
//     uint32_t reg_data = 0;
//     GPIO::set_bit(&reg_data, pin_in_register + 16, 1); // set write mask
//     GPIO::set_bit(&reg_data, pin_in_register, value);  // set actual pin bit
// #ifdef TEST_ENV
//     LOG_DEBUG("Setting pin value: chipnum: %d, pin_within_chip: %02d, register_type: %d, pin_in_register: %02d, pin_value: %d, reg_data: %X", gpio_chip_num, pin_within_chip, register_type, pin_in_register, value, reg_data);
// #endif
//     GPIO::write_to_data_register(gpio_chip_num, register_type, reg_data);
// }

// void GPIO::set_pin_in_var(uint8_t pin, bool value, uint32_t *var) {
//     if (pin > 159)
//     {
//         LOG_ERROR("Pin number out of range: %d", pin);
//         return;
//     }

//     // Determine the GPIO chip (each chip has 32 pins)
//     int gpio_chip_num = pin / 32;

//     // Determine whether it's the low or high register
//     int pin_within_chip = pin % 32;
//     int register_type = (pin_within_chip < 16) ? LOW : HIGH;

//     int pin_in_register = pin_within_chip % 16;
// }

void map_all_chip_registers()
{
    LOG_INFO("Mapping all registers");
    gpio_regs[0] = map_registers(GPIO0_BASE, GPIO0_SIZE);
    gpio_regs[1] = map_registers(GPIO1_BASE, GPIO1_SIZE);
    gpio_regs[2] = map_registers(GPIO2_BASE, GPIO2_SIZE);
    gpio_regs[3] = map_registers(GPIO3_BASE, GPIO3_SIZE);
    gpio_regs[4] = map_registers(GPIO4_BASE, GPIO4_SIZE);
}

int GPIO::init()
{
#ifdef TEST_ENV
    init_gpio_regs_testenv();
#endif
    LOG_INFO("Initializing GPIO");
    map_all_chip_registers();
    return 0;
}

int GPIO::exit()
{
    LOG_INFO("Uninitializing GPIO");
    return 0;
}