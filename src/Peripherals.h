#pragma once

#include <inttypes.h>
#include <stdbool.h>

#define BIT(x) (1UL << (x))

// Reset and Clock Control

struct rcc
{
    volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR, BDCR, CSR, AHBSTR, CFGR2;
};

#define RCC ((struct rcc*) 0x040021000)



// GPIO

struct Pin
{
    uint8_t bank;
    uint8_t number;
};

typedef struct Pin Pin;

struct gpio
{
    volatile uint32_t CR[2], IDR, ODR, BSRR, BRR, LCKR;
};

#define GPIO(bank) ((struct gpio*)(0x040010800 + 0x400 * (bank)))

enum
{
    GPIO_BANK_A,
    GPIO_BANK_B,
    GPIO_BANK_C,
    GPIO_BANK_D,
    GPIO_BANK_E,
    GPIO_BANK_F,
    GPIO_BANK_G,
};

enum
{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT_10M,
    GPIO_MODE_OUTPUT_2M,
    GPIO_MODE_OUTPUT_50M
};

enum
{
    GPIO_CNF_OUTPUT_GP_PP,
    GPIO_CNF_OUTPUT_GP_OD,
    GPIO_CNF_OUTPUT_AF_PP,
    GPIO_CNF_OUTPUT_AF_OD
};

enum
{
    GPIO_CNF_INPUT_ANALOG,
    GPIO_CNF_INPUT_FP,
    GPIO_CNF_INPUT_PP
};

void GPIOSetMode(Pin pin, uint8_t mode, uint8_t config);
void GPIOWrite(Pin pin, bool value);



// SysTick

struct systick
{
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};

#define SYSTICK ((struct systick*)(0xe000e010))

void SysTickInit(uint32_t ticks);
void SysTickHandler(void);
uint64_t GetTicks();
