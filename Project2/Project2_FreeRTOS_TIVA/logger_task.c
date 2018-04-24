/*
 * logger_task.c
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include "FreeRTOS.h"
#include "task.h"
#include "priorities.h"
#include "timers.h"

#include "my_tasks.h"
#include "my_uart.h"

#include "logger_task.h"
#include "communication_interface.h"

#define LOGGER_QUEUE_ITEMSIZE   (sizeof(LOG_QUEUE_MSG_T))
#define LOGGER_QUEUE_LENGTH     20

extern const char * const BOARD_TYPE;
extern const char * const OS;
extern const char * const CODE_VERSION;
extern const char * const UID;
extern const uint32_t BOARD_UID;

volatile uint8_t loggerTaskInitDone = 0;

static QueueHandle_t h_loggerQueue;
static TaskHandle_t h_loggerTask;

QueueHandle_t LoggerQueueHandle(QueueHandle_t handle, bool get)
{
    if(get)
        return h_loggerQueue;
    else
    {
        h_loggerQueue = handle;
        return h_loggerQueue;
    }
}

TaskHandle_t LoggerTaskHandle(TaskHandle_t handle, bool get)
{
    if(get)
        return h_loggerTask;
    else
    {
        h_loggerTask = handle;
        return h_loggerTask;
    }
}

static void logger_task_entry(void *params)
{
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);
    BaseType_t xResult;
    uint32_t notifiedValue = 0;
    while(1)
    {
        /* Wait to be notified of an interrupt. */
        xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
                           ULONG_MAX,        /* Clear all bits on exit. */
                           &notifiedValue, /* Stores the notified value. */
                           xMaxBlockTime);

        if( xResult == pdPASS )
        {
            /* A notification was received.  See which bits were set. */
            if(notifiedValue & EVENT_LOG_BOARD_TYPE)
            {
                COMM_MSG_T comm_msg;
                comm_msg.ID = CLIENT_INFO_BOARD_TYPE | BOARD_UID;
                strncpy(comm_msg.payload,BOARD_TYPE, sizeof(comm_msg.payload));
                comm_msg.checksum = getCheckSum(&comm_msg);
                COMM_SEND(comm_msg);
                printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
            }

            if(notifiedValue & EVENT_LOG_CODE_VERSION)
            {
                COMM_MSG_T comm_msg;
                comm_msg.ID = CLIENT_INFO_CODE_VERSION | BOARD_UID;
                strncpy(comm_msg.payload,CODE_VERSION, sizeof(comm_msg.payload));
                comm_msg.checksum = getCheckSum(&comm_msg);
                COMM_SEND(comm_msg);
                printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);

            }

            if(notifiedValue & EVENT_LOG_UID)
            {
                COMM_MSG_T comm_msg;
                comm_msg.ID = CLIENT_INFO_UID | BOARD_UID;
                strncpy(comm_msg.payload, UID, sizeof(comm_msg.payload));
                comm_msg.checksum = getCheckSum(&comm_msg);
                COMM_SEND(comm_msg);
                printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
            }

            if(notifiedValue & EVENT_LOG_HEARTBEAT)
            {
                COMM_MSG_T comm_msg;
//                static val = 0;
                comm_msg.ID = (HEARTBEAT | BOARD_UID);// + (val%500);
//                val++;
                strncpy(comm_msg.payload, "I AM ALIVE", sizeof(comm_msg.payload));
                comm_msg.checksum = getCheckSum(&comm_msg);
                COMM_SEND(comm_msg);
                printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
            }

            if(notifiedValue & EVENT_LOG_STATUS)
            {
                LOG_QUEUE_MSG_T q_msg;
                if(xQueueReceive(h_loggerQueue,&q_msg,xMaxBlockTime))
                {
                    COMM_MSG_T comm_msg;
                    comm_msg.ID = STATUS | BOARD_UID;
                    strncpy(comm_msg.payload,q_msg.msg, sizeof(comm_msg.payload));
                    comm_msg.checksum = getCheckSum(&comm_msg);
                    COMM_SEND(comm_msg);
                    printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_LOG_INFO)
            {
                LOG_QUEUE_MSG_T q_msg;
                if(xQueueReceive(h_loggerQueue,&q_msg,xMaxBlockTime))
                {
                    COMM_MSG_T comm_msg;
                    comm_msg.ID = INFO | BOARD_UID;
                    strncpy(comm_msg.payload,q_msg.msg, sizeof(comm_msg.payload));
                    comm_msg.checksum = getCheckSum(&comm_msg);
                    COMM_SEND(comm_msg);
                    printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_LOG_MSG)
            {
                LOG_QUEUE_MSG_T q_msg;
                if(xQueueReceive(h_loggerQueue,&q_msg,xMaxBlockTime))
                {
                    COMM_MSG_T comm_msg;
                    comm_msg.ID = MSG | BOARD_UID;
                    strncpy(comm_msg.payload,q_msg.msg, sizeof(comm_msg.payload));
                    comm_msg.checksum = getCheckSum(&comm_msg);
                    COMM_SEND(comm_msg);
                    printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_LOG_ERROR)
            {
                LOG_QUEUE_MSG_T q_msg;
                if(xQueueReceive(h_loggerQueue,&q_msg,xMaxBlockTime))
                {
                    COMM_MSG_T comm_msg;
                    comm_msg.ID = ERROR | BOARD_UID;
                    strncpy(comm_msg.payload,q_msg.msg, sizeof(comm_msg.payload));
                    comm_msg.checksum = getCheckSum(&comm_msg);
                    COMM_SEND(comm_msg);
                    printf("0x%x %d %s\n", GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID), GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }
        }
//        else
//        {
//            printf("LOGGER NOTIFICATION: TIMEOUT\n");
//        }
    }
}

uint8_t LoggerTask_init()
{
    /* Creating a Queue required for Logging the msg */
    QueueHandle_t h_loggerQ = xQueueCreate(LOGGER_QUEUE_LENGTH, LOGGER_QUEUE_ITEMSIZE);
    setLoggerQueueHandle(h_loggerQ);

    TaskHandle_t h_loggerTask;

    /*Initializing the communication interface*/
    COMM_INIT();
//    uint8_t data[32] = {0};
//    NRF_read_data(data, 32);
    /* Create the task*/
    if(xTaskCreate(logger_task_entry, (const portCHAR *)"Logger Task", 128, NULL,
                       tskIDLE_PRIORITY + PRIO_LOGGERTASK, &h_loggerTask) != pdTRUE)
    {
        return (1);
    }

    //Setting the logger task handle for future use
    setLoggerTaskHandle(h_loggerTask);
    /* Return the createtask ret value */
    return 0;
}




