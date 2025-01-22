#include "Peripherals.h"

void GPIOSetMode(Pin pin, uint8_t mode, uint8_t config)
{
    struct gpio* gpio = GPIO(pin.bank);
    
    // Handle input mode
    if (mode == GPIO_MODE_INPUT)
    {
        if (config == GPIO_CNF_INPUT_PULL_UP)
        {
            gpio->ODR |= (1 << pin.number);
            config = GPIO_CNF_INPUT_PULL_DOWN;
        }
        else if (config == GPIO_CNF_INPUT_PULL_DOWN)
        {
            gpio->ODR &= ~(1 << pin.number);
        }
    }

    // If configuration register is CRH
    volatile uint32_t* CR = &(gpio->CR[0]);
    uint8_t pinNumber = pin.number;

    if (pinNumber >= 8)
    {
        CR = &(gpio->CR[1]);
        pinNumber -= 8;
    }

    *CR &= ~(15U << (pinNumber * 4));

    *CR |= (mode & 3) << (pinNumber * 4);
    *CR |= (config & 3) << (pinNumber * 4 + 2);
}

void GPIOWrite(Pin pin, bool value)
{
    struct gpio* gpio = GPIO(pin.bank);

    if (value)
        gpio->BSRR = (1U << pin.number);
    else
        gpio->BSRR = (1U << pin.number) << 16;
}

bool GPIORead(Pin pin)
{
    struct gpio* gpio = GPIO(pin.bank);

    return (gpio->IDR & (1 << pin.number));
}

static volatile uint64_t s_Ticks;

void SysTickInit(uint32_t ticks)
{
    if ((ticks - 1) > 0xffffff)
        return;

    SYSTICK->LOAD = ticks - 1;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = BIT(0) | BIT(1) | BIT(2);
}

void SysTickHandler(void)
{
    s_Ticks++;
}

uint64_t GetTicks()
{
    return s_Ticks;
}
