#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
    UART_NUMBER_1,
    UART_NUMBER_2,
    UART_NUMBER_3
} UARTNumber;

typedef enum
{
    UART_ERROR,
    UART_OK,
    UART_BUSY
} UARTStatus;

typedef struct
{
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
} UARTRegisters;

typedef struct UART
{
    UARTRegisters* registers;

    // RX
    volatile bool RXStatus;
    uint8_t* RXBuffer;
    uint16_t RXBufferSize;
    volatile uint16_t nReceived;
    void(*USARTReceiveCallback)(struct UART*);

    // TX
    volatile bool TXStatus;
    uint8_t* TXBuffer;
    uint16_t TXBufferSize;
    volatile uint16_t nSent;
    void(*USARTTransmitCallback)(struct UART*);
} UART;



typedef void (*UARTCallback)(UART*);

UART* UARTInit(UARTNumber uartNumber, uint32_t baudRate);

UARTStatus UARTReceiveByte(const UART* uart, uint8_t* byte);
UARTStatus UARTTransmitByte(const UART* uart, uint8_t byte);

UARTStatus UARTReceive(const UART* uart, uint8_t* buffer, uint16_t bufferSize);
UARTStatus UARTTransmit(const UART* uart, uint8_t* buffer, uint16_t bufferSize);

UARTStatus UARTReceiveInterrupt(UART* uart, uint8_t* buffer, uint16_t bufferSize);
UARTStatus UARTTransmitInterrupt(UART* uart, uint8_t* buffer, size_t bufferSize);

// When DMA Receive/Transmit is enabled you should not use any other Receive/Transmit method
// Received/Transmited data should be handled with DMA
UARTStatus UARTEnableDMAReceive(UART* uart);
UARTStatus UARTDisableDMAReceive(UART* uart);
UARTStatus UARTEnableDMATransmit(UART* uart);
UARTStatus UARTDisableDMATransmit(UART* uart);

void UARTSetReceiveCompleteCallback(UART* uart, UARTCallback callback);
void UARTSetTransmitCompleteCallback(UART* uart, UARTCallback callback);
