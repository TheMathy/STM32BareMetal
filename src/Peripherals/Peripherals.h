#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#define CPU_FREQUENCY 8000000
#define BIT(x) (1UL << (x))

// Reset and Clock Control

struct rcc
{
    volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR, BDCR, CSR;
};

#define RCC ((struct rcc*) 0x040021000)



// GPIO

typedef struct Pin
{
    uint8_t bank;
    uint8_t number;
} Pin;

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



// AFIO

struct afio
{
    volatile uint32_t EVCR, MAPR, EXTICR1, EXTICR2, EXTICR3, EXTICR4, RESERVED, MAPR2;
};

#define AFIO ((struct afio*)(0x040010000))



// NVIC

void NVICEnableInterrupt(uint8_t positon);
void NVICDisableInterrupt(uint8_t positon);



// SysTick

void SysTickInit(uint32_t ticks);
void SysTickHandler(void);
uint64_t GetTicks();



// Timer

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

void TimerEnable(uint8_t timerNumber, uint16_t prescaler, uint16_t autoReload);
void TimerStart(uint8_t timerNumber);
void TimerStop(uint8_t timerNumber);

void TimerEnablePWM(uint8_t timerNumber, uint8_t channel);
void TimerSetPWMDutyCycle(uint8_t timerNumber, uint8_t channel, uint16_t ccValue);





