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
#include "communication_setup.h"
#include "comm_sender_task.h"
#include "sonar_sensor_task.h"
#include "camera_interface.h"

//#define CLOCK_FREQ 120000000
#define CLOCK_FREQ 16000000
uint32_t g_sysClock = CLOCK_FREQ;


void send_boardIdentification()
{
    NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_BOARD_TYPE);
    NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_UID);
    NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_CODE_VERSION);
}

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

   CommTask_init();
   CameraInit();

   send_boardIdentification();

   heartbeat_start(1000, 500);

   if(SonarSensorTask_init())
   {
       printf("[ERROR] %s\n",__FUNCTION__);
       while(1);
   }


   printf("SUCCESS - All tasks are created. Starting scheduler....\n");

   vTaskStartScheduler();

   while(1);


}
