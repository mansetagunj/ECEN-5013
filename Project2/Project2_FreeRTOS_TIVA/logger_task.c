/*
 * logger_task.c
 *
 *  Created on: 22-Apr-2018
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

#include "logger_task.h"


static void task1_2hz(void *params)
{
}

uint8_t LoggerTask_init()
{
    configASSERT(workerTaskInitDone == 1);
    /* Create the task*/
    if(xTaskCreate(task1_2hz, (const portCHAR *)"Logger Task", 128, NULL,
                       tskIDLE_PRIORITY + PRIO_LOGGERTASK, NULL) != pdTRUE)
    {
        return (1);
    }

    /* Return the createtask ret value */
    return 0;
}




