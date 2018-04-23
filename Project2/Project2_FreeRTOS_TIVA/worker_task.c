/*
 * worker_task.c
 *
 *  Created on: 06-Apr-2018
 *      Author: Gunj Manseta
 */
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "priorities.h"

#include "worker_task.h"
#include "my_uart.h"

#define LED_D1_PORT    GPIO_PORTN_BASE
#define LED_D2_PORT    GPIO_PORTN_BASE

#define LED_D1_PIN     GPIO_PIN_1
#define LED_D2_PIN     GPIO_PIN_0


#define WORKER_QUEUE_ITEMSIZE   (sizeof(WORKER_NOTIFY_MSG_T))
#define WORKER_QUEUE_LENGTH     10

#define MYTASKSTACKSIZE 128

volatile uint8_t workerTaskInitDone = 0;

QueueHandle_t workerQueueHandle(QueueHandle_t handle, bool get)
{
    static QueueHandle_t h_workerQueue;
    if(get)
        return h_workerQueue;
    else
    {
        h_workerQueue = handle;
        return h_workerQueue;
    }
}

TaskHandle_t workerTaskHandle(TaskHandle_t handle, bool get)
{
    static TaskHandle_t h_workerTask;
    if(get)
        return h_workerTask;
    else
    {
        h_workerTask = handle;
        return h_workerTask;
    }
}


static void workerTask(void *params)
{
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);
    BaseType_t xResult;
    uint32_t notifiedValue = 0;
    uint32_t ledValue = LED_D1_PIN | LED_D2_PIN;
    WORKER_NOTIFY_MSG_T notificationMSG;
    while(1)
    {
        /* Wait to be notified of an interrupt. */
        xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
                           ULONG_MAX,        /* Clear all bits on exit. */
                           &notifiedValue, /* Stores the notified value. */
                           xMaxBlockTime );

        if( xResult == pdPASS )
        {
         /* A notification was received.  See which bits were set. */
         if(notifiedValue & TOGGLE_LED)
         {
             ledValue ^= (LED_D1_PIN | LED_D2_PIN);
             GPIOPinWrite(LED_D1_PORT, LED_D1_PIN | LED_D2_PIN, ledValue);
         }

         if(notifiedValue & LOG_STRING)
         {
             /* Get the msg from LOG_QUEUE*/
             if(xQueueReceive(getWorkerQueueHandle(),&notificationMSG,xMaxBlockTime))
             {
                 printf("NOTIFIED MSG - %s TICK:%u\n",notificationMSG.msg,notificationMSG.tickcount);
             }
             else
             {
                 puts("ERROR - WORKER - QUEUE RECV\n");
             }
         }
        }
    }

}

uint8_t WorkerTask_init()
{
    /* Configure the GPIO pins*/
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    MAP_GPIOPinTypeGPIOOutput(LED_D2_PORT, LED_D2_PIN | LED_D1_PIN);

    /* Creating a Queue required for Logging the msg */
    QueueHandle_t h_workerQ = xQueueCreate(WORKER_QUEUE_LENGTH, WORKER_QUEUE_ITEMSIZE);
    setWorkerQueueHandle(h_workerQ);

    TaskHandle_t h_workerTask;
    /* Create the task*/
    if(xTaskCreate(workerTask, (const portCHAR *)"WorkerTask", MYTASKSTACKSIZE, NULL,
                       tskIDLE_PRIORITY + PRIO_WORKERTASK, &h_workerTask) != pdTRUE)
    {
        return 1;
    }

    setWorkerTaskHandle(h_workerTask);
    workerTaskInitDone = 1;
    /* Return the createtask ret value */
    return 0;
}

