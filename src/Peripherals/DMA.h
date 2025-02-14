#pragma once

#include <inttypes.h>
#include <stdbool.h>

typedef enum
{
    DMA_NUMBER_1,
    DMA_NUMBER_2
} DMANumber;

typedef enum
{
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_CHANNEL_4,
    DMA_CHANNEL_5,
    DMA_CHANNEL_6,
    DMA_CHANNEL_7
} DMAChannelNumber;

typedef enum
{
    DMA_DIRECTION_PER_TO_MEM,
    DMA_DIRECTION_MEM_TO_PER
} DMADirection;

typedef void (*DMACallback)(DMAChannelNumber);

void DMAInit(DMANumber dmaNumber);
void DMAConfigChannel(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber, void* perPtr, void* memPtr, uint16_t memSize, DMADirection dmaDirection, bool circular);
void DMAEnableChannel(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber);
void DMADisableChannel(DMANumber dmaNumber, DMAChannelNumber dmaChannelNumber);

void DMASetHalfTransferCallback(DMANumber dmaNumber, DMACallback callback);
void DMASetTransferCompleteCallback(DMANumber dmaNumber, DMACallback callback);
void DMASetTransferErrorCallback(DMANumber dmaNumber, DMACallback callback);
