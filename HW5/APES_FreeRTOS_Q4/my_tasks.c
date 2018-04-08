/*
 * my_tasks.c
 *
 *  Created on: 05-Apr-2018
 *      Author: Gunj Manseta
 */


#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "priorities.h"
#include "timers.h"

#include "my_tasks.h"
#include "my_uart.h"
#include "worker_task.h"

#define MYTASKSTACKSIZE 128

#define NUM_OF_TIMERS   2
TimerHandle_t timer_handles[NUM_OF_TIMERS];


//#define configASSERT( x ) if( ( x ) == pdFALSE ) { taskDISABLE_INTERRUPTS(); for( ;; ); }


void vTimerCallback(TimerHandle_t h_timer)
{
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);
    //Timer handle for 2hz task
    if(h_timer == timer_handles[0])
    {
        //Notify Worker Task with TOGGLE_LED
        xTaskNotify(getWorkerTaskHandle(),TOGGLE_LED,eSetBits);

    }
    //Timer handle for 4hz task
    else if(h_timer == timer_handles[1])
    {
        WORKER_NOTIFY_MSG_T notifyingMsg;
        memcpy(notifyingMsg.msg,"LOG_STRING from Timer",sizeof(notifyingMsg.msg));
        notifyingMsg.tickcount = xTaskGetTickCount();
        //Enqueue the worker queue with a new msg
        if(xQueueSend(getWorkerQueueHandle(), &notifyingMsg,xMaxBlockTime) == pdPASS)
        {
            //Notify Worker Task with LOG_STRING
            xTaskNotify(getWorkerTaskHandle(),LOG_STRING,eSetBits);
        }
        else
        {
            puts("ERROR - TIMER HANDLE - QUEUE SEND\n");
        }
    }
}

static void task1_2hz(void *params)
{
    timer_handles[0] = xTimerCreate("Timer2HZ", pdMS_TO_TICKS(500) , pdTRUE,  (void*)0, vTimerCallback);

    if(timer_handles[0] == NULL)
    {
        while(1);
    }

    if((xTimerStart(timer_handles[0], 0)) != pdTRUE)
    {
        while(1);
    }

    /* The only work of this task was to initialize a timer and start it. We are suspending the task now*/
    vTaskSuspend(NULL);

}

uint8_t Task1_init()
{
    configASSERT(workerTaskInitDone == 1);
    /* Create the task*/
    if(xTaskCreate(task1_2hz, (const portCHAR *)"task_2hz", MYTASKSTACKSIZE, NULL,
                       tskIDLE_PRIORITY + PRIO_MY_TASK1, NULL) != pdTRUE)
    {
        return (1);
    }

    /* Return the createtask ret value */
    return 0;
}

static void task2_4hz(void *params)
{
    timer_handles[1] = xTimerCreate("Timer4HZ", pdMS_TO_TICKS(250) , pdTRUE,  (void*)0, vTimerCallback);

    if(timer_handles[1] == NULL)
    {
        while(1);
    }

    if((xTimerStart(timer_handles[1], 0)) != pdTRUE)
    {
        while(1);
    }

    /* The only work of this task was to initialize a timer and start it. We are suspending the task now*/
    vTaskSuspend(NULL);
}

uint8_t Task2_init()
{
    configASSERT(workerTaskInitDone == 1);
    /* Create the task*/
    if(xTaskCreate(task2_4hz, (const portCHAR *)"task_4hz", MYTASKSTACKSIZE, NULL,
                       tskIDLE_PRIORITY + PRIO_MY_TASK2, NULL) != pdTRUE)
    {
        return 1;
    }

    /* Return the createtask ret value */
    return 0;
}



