#include "Peripherals/Peripherals.h"
#include "Peripherals/UART.h"
#include "Peripherals/DMA.h"

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

static uint8_t rxBuffer[8];
static uint8_t buffer[8];
static bool dataReady = 0;

void DMA1HalfTransferCallback(DMAChannelNumber dmaChannelNumber)
{
    if (dmaChannelNumber == DMA_CHANNEL_5)
        memcpy(buffer, rxBuffer, 4);
}

void DMA1TransferCompleteCallback(DMAChannelNumber dmaChannelNumber)
{
    if (dmaChannelNumber == DMA_CHANNEL_5)
    {
        memcpy(&buffer[4], &rxBuffer[4], 4);
        dataReady = 1;
    }
}

int main()
{
    // Enable GPIOA and GPIOB
    RCC->APB2ENR |= BIT(GPIO_BANK_A + 2);
    RCC->APB2ENR |= BIT(GPIO_BANK_B + 2);

    SysTickInit(CPU_FREQUENCY / 1000);

    Pin greenLED = {GPIO_BANK_A, 6};
    Pin blueLED = {GPIO_BANK_A, 5};

    GPIOSetMode(greenLED, GPIO_MODE_OUTPUT_10M, GPIO_CNF_OUTPUT_AF_PP);
    GPIOSetMode(blueLED, GPIO_MODE_OUTPUT_10M, GPIO_CNF_OUTPUT_GP_PP);

    Pin button1 = {GPIO_BANK_B, 14};
    Pin button2 = {GPIO_BANK_B, 15};

    GPIOSetMode(button1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOATING);
    GPIOSetMode(button2, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOATING);

    TimerEnable(TIMER_3, 80, 200);
    TimerEnablePWM(TIMER_3, TIMER_CHANNEL_1);
    TimerSetPWMDutyCycle(TIMER_3, TIMER_CHANNEL_1, 8);
    TimerStart(TIMER_3);

    // Enable AFIO and remap UART1 to PB6 and PB7
    RCC->APB2ENR |= BIT(0);
    AFIO->MAPR |= BIT(2);

    Pin usart1TX = {GPIO_BANK_B, 6};
    Pin usart1RX = {GPIO_BANK_B, 7};
    
    GPIOSetMode(usart1TX, GPIO_MODE_OUTPUT_10M, GPIO_CNF_OUTPUT_AF_PP);
    GPIOSetMode(usart1RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOATING);
    

    UART* uart1 = UARTInit(UART_NUMBER_1, 115200);

    // DMA
    DMAInit(DMA_NUMBER_1);
    DMAConfigChannel(DMA_NUMBER_1, DMA_CHANNEL_5, (void*)&(uart1->registers->DR), rxBuffer, 8, DMA_DIRECTION_PER_TO_MEM, 1);

    DMAEnableChannel(DMA_NUMBER_1, DMA_CHANNEL_5);
    DMASetHalfTransferCallback(DMA_NUMBER_1, DMA1HalfTransferCallback);
    DMASetTransferCompleteCallback(DMA_NUMBER_1, DMA1TransferCompleteCallback);

    DMAConfigChannel(DMA_NUMBER_1, DMA_CHANNEL_4, (void*)&(uart1->registers->DR), buffer, 8, DMA_DIRECTION_MEM_TO_PER, 0);

    UARTEnableDMAReceive(uart1);
    UARTEnableDMATransmit(uart1);

    uint64_t startTime = GetTicks();
    uint32_t current = 8;

    bool button1Pressed = 0;
    bool button2Pressed = 0;
    bool status = 1;

    while (1)
    {
        bool button1Status = !GPIORead(button1);
        bool button2Status = !GPIORead(button2);

        // Decrease brightness button
        if (button1Status && !button1Pressed)
        {
            current /= 2;
            TimerSetPWMDutyCycle(TIMER_3, TIMER_CHANNEL_1, current);

            button1Pressed = 1;
        }
        else if (!button1Status)
            button1Pressed = 0;

        // Increase brightness button
        if (button2Status && !button2Pressed)
        {    
            if (current == 0)
                current = 1;
            else
                current *= 2;

            if (current > 200)
                current = 200;

            TimerSetPWMDutyCycle(TIMER_3, TIMER_CHANNEL_1, current);

            button2Pressed = 1;
        }
        else if (!button2Status)
            button2Pressed = 0;
        
        
        // Blink blue LED
        if (GetTicks() > startTime + 500)
        {
            //GPIOWrite(blueLED, status);
            status = !status;
            startTime = GetTicks();
        }

        if (dataReady)
        {
            DMADisableChannel(DMA_NUMBER_1, DMA_CHANNEL_4);
            DMAConfigChannel(DMA_NUMBER_1, DMA_CHANNEL_4, (void*)&(uart1->registers->DR), buffer, 8, DMA_DIRECTION_MEM_TO_PER, 0);
            DMAEnableChannel(DMA_NUMBER_1, DMA_CHANNEL_4);
            dataReady = 0;
        }
    }

    return 0;
}
