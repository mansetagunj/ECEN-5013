/*
 * main.c
 *
 *  Created on: 05-Apr-2018
 *      Author: Gunj Manseta
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

#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"
//#include "semphr.h"

#include "my_tasks.h"
#include "my_uart.h"


void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    printf("\nSTACK ERROR - TASK: %s\n",pcTaskName);
    while(1)
    {
    }
}


#define CLOCK_FREQ 120000000
uint32_t g_sysClock = CLOCK_FREQ;

int main()
{
    // Set the clocking to run directly from the crystal at 120MHz.
    g_sysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), g_sysClock);

    UART0_config(BAUD_115200);

    printf("\n----- GUNJ HW5 Q3 --------\n");

    if(Task1_init() != 0)
    {
        printf("ERROR - TASK1 INIT\n");
        while(1);
    }

    if(Task2_init() != 0)
    {
        printf("ERROR - TASK2 INIT\n");
        while(1);
    }

    printf("SUCCESS - All tasks are created. Starting scheduler....\n");

    vTaskStartScheduler();

    while(1);

    return 0;
}
