#include "Peripherals/Peripherals.h"

extern int main();

__attribute__((naked, noreturn)) void _reset(void)
{
    extern long _sbss, _ebss, _sdata, _edata, _sidata;

    // Set .bst to 0
    for (long *dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;

    // Copy .data to RAM
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;)
        *dst++ = *src++;

    main();

    while (1)
        (void) 0;
}

extern void _estack(void);

void USART1IRQHandler();
void USART2IRQHandler();
void USART3IRQHandler();

void DMA1Channel1IRQHandler();
void DMA1Channel2IRQHandler();
void DMA1Channel3IRQHandler();
void DMA1Channel4IRQHandler();
void DMA1Channel5IRQHandler();
void DMA1Channel6IRQHandler();
void DMA1Channel7IRQHandler();

void DMA2Channel1IRQHandler();
void DMA2Channel2IRQHandler();
void DMA2Channel3IRQHandler();
void DMA2Channel4IRQHandler();
void DMA2Channel5IRQHandler();

__attribute__((section(".vectors"))) void (*const tab[16 + 68])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTickHandler,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DMA1Channel1IRQHandler, DMA1Channel2IRQHandler, DMA1Channel3IRQHandler, DMA1Channel4IRQHandler, DMA1Channel5IRQHandler,
    DMA1Channel6IRQHandler, DMA1Channel7IRQHandler, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, USART1IRQHandler, USART2IRQHandler, USART3IRQHandler, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, DMA2Channel1IRQHandler, DMA2Channel2IRQHandler, DMA2Channel3IRQHandler, DMA2Channel4IRQHandler, DMA2Channel5IRQHandler
};
