/*
 * application_hooks.c
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef APPLICATION_HOOKS_C_
#define APPLICATION_HOOKS_C_

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
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

#endif /* APPLICATION_HOOKS_C_ */
