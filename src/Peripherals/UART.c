#include "UART.h"

#include "Peripherals/Peripherals.h"

static UART uart1 = {NULL};
static UART uart2 = {NULL};
static UART uart3 = {NULL};

static void USARTIRQHandler(UART* uart)
{
    // Read data register not empty
    if (uart->registers->SR & BIT(5))
    {
        if (uart->RXStatus)
        {
            uart->RXBuffer[uart->nReceived] = uart->registers->DR;
            uart->nReceived++;

            // Receiving complete
            if (uart->nReceived >= uart->RXBufferSize)
            {
                uart->RXStatus = 0;
                uart->registers->CR1 &= ~(BIT(5));

                if (uart->USARTReceiveCallback)
                    uart->USARTReceiveCallback(&uart1);
            }
        } 
    }

    // Transmit data regiser empty
    if (uart->registers->SR & BIT(7))
    {
        if (uart->TXStatus && uart->nSent < uart->TXBufferSize)
        {
            uart->registers->DR = uart->TXBuffer[uart->nSent];
            uart->nSent++;
        }
    }

    // Transmission complete
    if (uart->registers->SR & BIT(6))
    {
        if (uart->TXStatus && uart->nSent >= uart->TXBufferSize)
        {
            uart->TXStatus = 0;
            uart->registers->CR1 &= ~(BIT(6) | BIT(7));

            if (uart->USARTTransmitCallback)
                uart->USARTTransmitCallback(&uart1);
        }
    }
}

void USART1IRQHandler() { USARTIRQHandler(&uart1); }
void USART2IRQHandler() { USARTIRQHandler(&uart2); }
void USART3IRQHandler() { USARTIRQHandler(&uart3); }

// If UARTX is already initialized, returns pointer to it
UART* UARTInit(UARTNumber uartNumber, uint32_t baudRate)
{
    UART* uart;

    if (uartNumber == UART_NUMBER_1)
    {
        if (uart1.registers)
            return &uart1;

        RCC->APB2ENR |= BIT(14);
        uart1.registers = (UARTRegisters*)(0x040013800);

        NVICEnableInterrupt(37);

        uart = &uart1;
    }
    else if (uartNumber == UART_NUMBER_2)
    {
        if (uart2.registers)
            return &uart2;

        uart2.registers = (UARTRegisters*)(0x040004400);
        RCC->APB1ENR |= BIT(17);

        NVICEnableInterrupt(38);

        uart = &uart2;
    }
    else if (uartNumber == UART_NUMBER_3)
    {
        if (uart3.registers)
            return &uart3;

        uart3.registers = (UARTRegisters*)(0x040004800);
        RCC->APB1ENR |= BIT(18);

        NVICEnableInterrupt(39);

        uart = &uart3;
    }
    else
    {
        return NULL;
    }

    uart->RXStatus = 0;
    uart->RXBuffer = NULL;
    uart->RXBufferSize = 0;
    uart->nReceived = 0;
    uart->USARTReceiveCallback = NULL;

    uart->TXStatus = 0;
    uart->TXBuffer = NULL;
    uart->TXBufferSize = 0;
    uart->nSent = 0;
    uart->USARTTransmitCallback = NULL;

    // Reset CR1 register
    uart->registers->CR1 = 0;

    // Set baud rate
    uart->registers->BRR = CPU_FREQUENCY / baudRate;

    // Enable UART, receive, transmit
    uart->registers->CR1 |= BIT(13) | BIT(3) | BIT(2);
    
    return uart;
}

UARTStatus UARTReceiveByte(const UART* uart, uint8_t* byte)
{
    if (uart->RXStatus)
        return UART_BUSY;

    if (byte == NULL)
        return UART_ERROR;


    // Wait for date in data register
    while (!(uart->registers->SR & BIT(5)));

    *byte = uart->registers->DR;
    
    return UART_OK;
}

UARTStatus UARTTransmitByte(const UART* uart, uint8_t byte)
{
    if (uart->TXStatus)
        return UART_BUSY;

    // Set byte to TDR
    uart->registers->DR = byte;

    // Wait for date to be sent
    while (!(uart->registers->SR & BIT(6)));

    return UART_OK;
}

UARTStatus UARTReceive(const UART* uart, uint8_t* buffer, uint16_t bufferSize)
{
    if (uart->RXStatus)
        return UART_BUSY;

    if (buffer == NULL || bufferSize == 0)
        return UART_ERROR;

    for (size_t i = 0; i < bufferSize; i++)
    {
        // Wait for byte to be in the dara register
        while (!(uart->registers->SR & BIT(5)));

        buffer[i] = uart->registers->DR;
    }

    return UART_OK;
}

UARTStatus UARTTransmit(const UART* uart, uint8_t* buffer, uint16_t bufferSize)
{
    if (uart->TXStatus)
        return UART_BUSY;

    if (buffer == NULL || bufferSize == 0)
        return UART_ERROR;

    for (size_t i = 0; i < bufferSize; i++)
    {
        // Wait for data transfer to the shift register
        while (!(uart->registers->SR & BIT(7)));

        uart->registers->DR = buffer[i];
    }

    // Wait for transmission to complete
    while (!(uart->registers->SR & BIT(6)));

    return UART_OK;
}

UARTStatus UARTReceiveInterrupt(UART* uart, uint8_t* buffer, uint16_t bufferSize)
{
    if (uart->RXStatus)
        return UART_BUSY;

    if (buffer == NULL || bufferSize == 0)
        return UART_ERROR;

    uart->RXStatus = 1;

    uart->RXBuffer = buffer;
    uart->RXBufferSize = bufferSize;
    uart->nReceived = 0;
    
    // Enable receive interrupt
    uart->registers->CR1 |= BIT(5);

    return UART_OK;
}

UARTStatus UARTTransmitInterrupt(UART* uart, uint8_t* buffer, size_t bufferSize)
{
    if (uart->TXStatus)
        return UART_BUSY;

    if (buffer == NULL || bufferSize == 0)
        return UART_ERROR;

    uart->TXStatus = 1;

    uart->TXBuffer = buffer;
    uart->TXBufferSize = bufferSize;
    uart->nSent = 0;

    // Send first byte
    uart1.registers->DR = uart->TXBuffer[0];
    uart->nSent++;

    // Enable transmit interrupts
    uart->registers->CR1 |= BIT(6) | BIT(7);

    return UART_OK;
}

void UARTSetReceiveCompleteCallback(UART* uart, UARTCallback callback)
{
    uart->USARTReceiveCallback = callback;
}

void UARTSetTransmitCompleteCallback(UART* uart, UARTCallback callback)
{
    uart->USARTTransmitCallback = callback;
}
