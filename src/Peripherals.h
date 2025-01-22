#pragma once

#include <inttypes.h>
#include <stdbool.h>

#define BIT(x) (1UL << (x))

// Reset and Clock Control

struct rcc
{
    volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR, BDCR, CSR;
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
    GPIO_BANK_G
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
    GPIO_CNF_INPUT_FLOATING,
    GPIO_CNF_INPUT_PULL_DOWN,
    GPIO_CNF_INPUT_PULL_UP
};

void GPIOSetMode(Pin pin, uint8_t mode, uint8_t config);
void GPIOWrite(Pin pin, bool value);
bool GPIORead(Pin pin);



// SysTick

struct systick
{
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};

#define SYSTICK ((struct systick*)(0xe000e010))

void SysTickInit(uint32_t ticks);
void SysTickHandler(void);
uint64_t GetTicks();



// Timer

struct timer
{
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2, CCER, CNT, PSC, ARR, RESERVED, CCR1, CCR2, CCR3, CCR4, RESERVED2, DCR, DMAR; 
};

#define TIMER(number) ((struct timer*)(0x040000000 + 0x400 * (number)))

enum
{
    TIMER_2,
    TIMER_3,
    TIMER_4
};

enum
{
    TIMER_CHANNEL_1,
    TIMER_CHANNEL_2,
    TIMER_CHANNEL_3,
    TIMER_CHANNEL_4
};

void TimerConfig(uint8_t timerNumber, uint16_t prescaler, uint16_t autoReload);
void TimerStart(uint8_t timerNumber);
void TimerStop(uint8_t timerNumber);

void TimerEnablePWM(uint8_t timerNumber, uint8_t channel);
void TimerSetPWMDutyCycle(uint8_t timerNumber, uint8_t channel, uint16_t ccValue);
