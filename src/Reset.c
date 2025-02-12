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

__attribute__((section(".vectors"))) void (*const tab[16 + 68])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTickHandler,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, USART1IRQHandler, USART2IRQHandler, USART3IRQHandler
};
