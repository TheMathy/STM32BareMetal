#include "Peripherals.h"

#include <inttypes.h>
#include <stdbool.h>

int main()
{
    // Enable GPIOA and GPIOB
    RCC->APB2ENR |= BIT(GPIO_BANK_A + 2);
    RCC->APB2ENR |= BIT(GPIO_BANK_B + 2);
    SysTickInit(8000000 / 1000);

    Pin greenLED = {GPIO_BANK_A, 6};
    Pin blueLED = {GPIO_BANK_A, 5};

    GPIOSetMode(greenLED, GPIO_MODE_OUTPUT_10M, GPIO_CNF_OUTPUT_GP_PP);
    GPIOSetMode(blueLED, GPIO_MODE_OUTPUT_10M, GPIO_CNF_OUTPUT_GP_PP);

    GPIOWrite(blueLED, 1);

    Pin button1 = {GPIO_BANK_B, 14};
    Pin button2 = {GPIO_BANK_B, 15};

    GPIOSetMode(button1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOATING);
    GPIOSetMode(button2, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOATING);
    
    uint64_t startTime = GetTicks();
    bool status = 1;

    while (1)
    {
        if (!GPIORead(button1) || !GPIORead(button2))
        {
            GPIOWrite(blueLED, 1);
        }
        else
        {
            GPIOWrite(blueLED, 0);
        }

        if (GetTicks() > startTime + 500)
        {
            GPIOWrite(greenLED, status);
            
            status = !status;
            startTime = GetTicks();
        }
    }

    return 0;
}
