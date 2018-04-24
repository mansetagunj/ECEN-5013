/*
 * application.c
 *
 *  Created on: 22-Apr-2018
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

#include "my_uart.h"
#include "heartbeat.h"
#include "application.h"
#include "logger_task.h"

#define CLOCK_FREQ 120000000
uint32_t g_sysClock = CLOCK_FREQ;


void application_run()
{
    // Set the clocking to run directly from the crystal at 120MHz.
   g_sysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                            SYSCTL_OSC_MAIN |
                                            SYSCTL_USE_PLL |
                                            SYSCTL_CFG_VCO_480), g_sysClock);

//   UART0_config(BAUD_115200);
   UART0_config(BAUD_921600);
   ROM_IntMasterEnable();
   printf("\n----- GUNJ Project2 --------\n");

   LoggerTask_init();

   heartbeat_start(1000, 500);


//   if(WorkerTask_init() != 0)
//   {
//       printf("ERROR - WORKER TASK INIT\n");
//       while(1);
//   }

   printf("SUCCESS - All tasks are created. Starting scheduler....\n");

   vTaskStartScheduler();

   while(1);


}
