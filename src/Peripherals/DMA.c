#include "DMA.h"

#include "Peripherals/Peripherals.h"

typedef struct
{
    volatile uint32_t CCR, CNDTR, CPAR, CMAR, RESERVED;
} DMAChannel;

typedef struct
{
    volatile uint32_t ISR, IFCR;
    volatile DMAChannel channels[7];
} DMARegisters;

#define DMA1 ((DMARegisters*)(0x040020000))
#define DMA2 ((DMARegisters*)(0x040020400))

static DMACallback DMA1HalfTransferCallback = NULL;
static DMACallback DMA1TransferCompleteCallback = NULL;
static DMACallback DMA1TransferErrorCallback = NULL;

static DMACallback DMA2HalfTransferCallback = NULL;
static DMACallback DMA2TransferCompleteCallback = NULL;
static DMACallback DMA2TransferErrorCallback = NULL;

static void DMAIRQHandler(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber)
{
    DMARegisters* dmaRegisters;

    if (dmaNumber == DMA_NUMBER_1)
        dmaRegisters = DMA1;
    else if (dmaNumber == DMA_NUMBER_2)
        dmaRegisters = DMA2;
    else
        return;

    // Half transfer complete
    if (dmaRegisters->ISR & BIT(4 * dmaChannelNumber + 2))
    {
        if (dmaNumber == DMA_NUMBER_1 && DMA1HalfTransferCallback)
            DMA1HalfTransferCallback(dmaChannelNumber);
        else if (dmaNumber == DMA_NUMBER_2 && DMA2HalfTransferCallback)
            DMA2HalfTransferCallback(dmaChannelNumber);

        dmaRegisters->IFCR |= BIT(4 * dmaChannelNumber + 2);
    }

    // Transfer complete
    if (dmaRegisters->ISR & BIT(4 * dmaChannelNumber + 1))
    {
        if (dmaNumber == DMA_NUMBER_1 && DMA1TransferCompleteCallback)
            DMA1TransferCompleteCallback(dmaChannelNumber);
        else if (dmaNumber == DMA_NUMBER_2 && DMA2TransferCompleteCallback)
            DMA2TransferCompleteCallback(dmaChannelNumber);

        dmaRegisters->IFCR |= BIT(4 * dmaChannelNumber + 1);
    }

    // Transfer error
    if (dmaRegisters->ISR & BIT(4 * dmaChannelNumber + 3))
    {
        if (dmaNumber == DMA_NUMBER_1 && DMA1TransferErrorCallback)
            DMA1TransferErrorCallback(dmaChannelNumber);
        else if (dmaNumber == DMA_NUMBER_2 && DMA2TransferErrorCallback)
            DMA2TransferErrorCallback(dmaChannelNumber);

        dmaRegisters->IFCR |= BIT(4 * dmaChannelNumber + 3);
    }
}

void DMA1Channel1IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_1); }
void DMA1Channel2IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_2); }
void DMA1Channel3IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_3); }
void DMA1Channel4IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_4); }
void DMA1Channel5IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_5); }
void DMA1Channel6IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_6); }
void DMA1Channel7IRQHandler() { DMAIRQHandler(DMA_NUMBER_1, DMA_CHANNEL_7); }

void DMA2Channel1IRQHandler() { DMAIRQHandler(DMA_NUMBER_2, DMA_CHANNEL_1); }
void DMA2Channel2IRQHandler() { DMAIRQHandler(DMA_NUMBER_2, DMA_CHANNEL_2); }
void DMA2Channel3IRQHandler() { DMAIRQHandler(DMA_NUMBER_2, DMA_CHANNEL_3); }
void DMA2Channel4IRQHandler() { DMAIRQHandler(DMA_NUMBER_2, DMA_CHANNEL_4); }
void DMA2Channel5IRQHandler() { DMAIRQHandler(DMA_NUMBER_2, DMA_CHANNEL_5); }

void DMAInit(DMANumber dmaNumber)
{
    if (dmaNumber == DMA_NUMBER_1)
        RCC->AHBENR |= BIT(0);
    else if (dmaNumber == DMA_NUMBER_2)
        RCC->AHBENR |= BIT(1);
}

void DMAConfigChannel(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber, void* perPtr, void* memPtr, uint16_t memSize, DMADirection dmaDirection, bool circular)
{
    volatile DMAChannel* dmaChannel;

    if (dmaNumber == DMA_NUMBER_1)
        dmaChannel = &DMA1->channels[dmaChannelNumber];
    else if (dmaNumber == DMA_NUMBER_2)
        dmaChannel = &DMA2->channels[dmaChannelNumber];
    else
        return;

    // Enable interupts
    dmaChannel->CCR |= BIT(1) | BIT(2) | BIT(3);

    // Direction
    if (dmaDirection == DMA_DIRECTION_PER_TO_MEM)
        dmaChannel->CCR &= ~BIT(4);
    else if (dmaDirection == DMA_DIRECTION_MEM_TO_PER)
        dmaChannel->CCR |= BIT(4);
    else
        return;

    // Circular
    if (circular)
        dmaChannel->CCR |= BIT(5);
    else
        dmaChannel->CCR &= ~BIT(5);

    // Disable peripheral increment mode
    dmaChannel->CCR &= ~BIT(6);

    // Enable memory increament
    dmaChannel->CCR |= BIT(7);

    // Set memory and peripheral size to 8 bits
    dmaChannel->CCR &= ~(BIT(8) | BIT(9) | BIT(10) | BIT(11));

    // Set priority to low
    // TODO: Settable priority
    dmaChannel->CCR &= ~(BIT(12) | BIT(13));

    // Disable memory to memory mode
    dmaChannel->CCR &= ~(BIT(14));

    dmaChannel->CNDTR = memSize;
    dmaChannel->CPAR = (uint32_t)perPtr;
    dmaChannel->CMAR = (uint32_t)memPtr;

    // Enable interrupt
    if (dmaNumber == DMA_NUMBER_1)
        NVICEnableInterrupt(11 + dmaChannelNumber);
    else if (dmaNumber == DMA_NUMBER_2)
        NVICEnableInterrupt(56 + dmaChannelNumber);
}

void DMAEnableChannel(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber)
{
    if (dmaNumber == DMA_NUMBER_1)
        DMA1->channels[dmaChannelNumber].CCR |= BIT(0);
    else if (dmaNumber == DMA_NUMBER_2)
        DMA2->channels[dmaChannelNumber].CCR |= BIT(0); 
}

void DMADisableChannel(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber)
{
    if (dmaNumber == DMA_NUMBER_1)
        DMA1->channels[dmaChannelNumber].CCR &= ~BIT(0);
    else if (dmaNumber == DMA_NUMBER_2)
        DMA2->channels[dmaChannelNumber].CCR &= ~BIT(0); 
}

void DMASetHalfTransferCallback(DMANumber dmaNumber, DMACallback callback)
{
    if (dmaNumber == DMA_NUMBER_1)
        DMA1HalfTransferCallback = callback;
    else if (dmaNumber == DMA_NUMBER_2)
        DMA2HalfTransferCallback = callback;
}

void DMASetTransferCompleteCallback(DMANumber dmaNumber, DMACallback callback)
{
    if (dmaNumber == DMA_NUMBER_1)
        DMA1TransferCompleteCallback = callback;
    else if (dmaNumber == DMA_NUMBER_2)
        DMA2TransferCompleteCallback = callback;
}

void DMASetTransferErrorCallback(DMANumber dmaNumber, DMACallback callback)
{
    if (dmaNumber == DMA_NUMBER_1)
        DMA1TransferErrorCallback = callback;
    else if (dmaNumber == DMA_NUMBER_2)
        DMA2TransferErrorCallback = callback;
}
