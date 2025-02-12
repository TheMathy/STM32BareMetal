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

UART* UARTInit(UARTNumber usartNumber, uint32_t baudRate);

UARTStatus UARTReceiveByte(const UART* usart, uint8_t* byte);
UARTStatus UARTTransmitByte(const UART* usart, uint8_t byte);

UARTStatus UARTReceive(const UART* usart, uint8_t* buffer, uint16_t bufferSize);
UARTStatus UARTTransmit(const UART* usart, uint8_t* buffer, uint16_t bufferSize);

UARTStatus UARTReceiveInterrupt(UART* usart, uint8_t* buffer, uint16_t bufferSize);
UARTStatus UARTTransmitInterrupt(UART* usart, uint8_t* buffer, size_t bufferSize);

void UARTSetReceiveCompleteCallback(UART* usart, UARTCallback callback);
void UARTSetTransmitCompleteCallback(UART* usart, UARTCallback callback);
