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

void TimerEnable(uint8_t timerNumber, uint16_t prescaler, uint16_t autoReload)
{
    struct timer* timer = TIMER(timerNumber);

    RCC->APB1ENR |= BIT(timerNumber);

    timer->CR1 &= ~(BIT(0));
    timer->CR1 &= ~(BIT(4));

    timer->PSC = prescaler;
    timer->ARR = autoReload - 1;
}

void TimerStart(uint8_t timerNumber)
{
    struct timer* timer = TIMER(timerNumber);
    timer->CR1 |= BIT(0);
}

void TimerStop(uint8_t timerNumber)
{
    struct timer* timer = TIMER(timerNumber);
    timer->CR1 &= ~(BIT(0));
}

void TimerEnablePWM(uint8_t timerNumber, uint8_t channel)
{
    struct timer* timer = TIMER(timerNumber);

    if (channel == TIMER_CHANNEL_1 || channel == TIMER_CHANNEL_2)
    {
        timer->CCMR1 &= ~(7U << (4 + 8 * channel));
        timer->CCMR1 |= (BIT(5 + 8 * channel) | BIT(6 + 8 * channel));
        timer->CCMR1 |= BIT(3 + 8 * channel);
    }
    else
    {
        timer->CCMR2 &= ~(7U << (4 + 8 * (channel - 2)));
        timer->CCMR2 |= (BIT(5 + 8 * (channel - 2)) | BIT(6 + 8 * (channel - 2)));
        timer->CCMR2 |= BIT(3 + 8 * (channel - 2));
    }

    timer->CCER &= ~(BIT(1 + channel * 4));
    timer->CCER |= BIT(0 + channel * 4);
}

void TimerSetPWMDutyCycle(uint8_t timerNumber, uint8_t channel, uint16_t ccValue)
{
    struct timer* timer = TIMER(timerNumber);

    *(&timer->CCR1 + channel) = ccValue;
}

void USARTEnable(uint8_t usartNumber, uint32_t baudRate)
{
    struct usart* usart;

    if (usartNumber == 1)
    {
        RCC->APB2ENR |= BIT(14);
        usart = (struct usart*)(0x040013800);
    }
    else if (usartNumber == 2)
    {
        usart = (struct usart*)(0x040004400);
        RCC->APB1ENR |= BIT(17);
    }
    else if (usartNumber == 3)
    {
        usart = (struct usart*)(0x040004800);
        RCC->APB1ENR |= BIT(18);
    }
    else
        return;
    
    usart->CR1 = 0;
    usart->BRR = CPU_FREQUENCY / baudRate;
    usart->CR1 |= BIT(13) | BIT(2) | BIT(3);
}

void USARTSendByte(uint8_t usartNumber, uint8_t byte)
{
    struct usart* usart;

    if (usartNumber == 1)
        usart = (struct usart*)(0x040013800);
    else if (usartNumber == 2)
        usart = (struct usart*)(0x040004400);
    else if (usartNumber == 3)
        usart = (struct usart*)(0x040004800);
    else
        return;

    usart->DR = byte;
    
    while (!(usart->SR & (1 << 6)));
}

uint8_t USARTReceiveByte(uint8_t usartNumber)
{
    struct usart* usart;

    if (usartNumber == 1)
        usart = (struct usart*)(0x040013800);
    else if (usartNumber == 2)
        usart = (struct usart*)(0x040004400);
    else if (usartNumber == 3)
        usart = (struct usart*)(0x040004800);
    else
        return 0;

    while (!(usart->SR & (1 << 5)));
            
    uint8_t byte = usart->DR;

    return byte;
}
