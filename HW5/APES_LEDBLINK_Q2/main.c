
/**
 * main.c
 * @Author - Gunj Manseta
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_ints.h"

#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "my_uart.h"

#define CLOCK_FREQ 120000000
uint32_t g_sysClock = CLOCK_FREQ;

int main(void)
{
    // Set the clocking to run directly from the crystal at 120MHz.
    g_sysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), g_sysClock);

    UART0_config(BAUD_115200);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);

    puts("PROJECT FOR GUNJ MANSETA - 04/05/2018\n");

    uint8_t led_val = GPIO_PIN_1 | GPIO_PIN_0 ;

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, led_val);

    while(1)
    {
        static uint32_t count = 0;
        led_val ^= (GPIO_PIN_1 | GPIO_PIN_0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, led_val);
        printf("LED BLINK COUNT: %u\n",++count);
        SysCtlDelay((g_sysClock/ (1000 * 3))*500);
    }

	//return 0;
}
