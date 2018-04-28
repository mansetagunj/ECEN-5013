/*
 * comm_sender_task.c
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

#include "communication_interface.h"
#include "comm_sender_task.h"

#define COMM_SENDER_QUEUE_ITEMSIZE   (sizeof(COMM_MSG_T))
#define COMM_SENDER_QUEUE_LENGTH     20

extern const char * const BOARD_TYPE;
extern const char * const OS;
extern const char * const CODE_VERSION;
extern const char * const UID;

volatile uint8_t comm_senderTaskInitDone = 0;
static QueueHandle_t h_comm_senderQueue;
static TaskHandle_t h_comm_senderTask;

QueueHandle_t Comm_senderQueueHandle(QueueHandle_t handle, bool get)
{
    if(get)
        return h_comm_senderQueue;
    else
    {
        h_comm_senderQueue = handle;
        return h_comm_senderQueue;
    }
}

TaskHandle_t Comm_senderTaskHandle(TaskHandle_t handle, bool get)
{
    if(get)
        return h_comm_senderTask;
    else
    {
        h_comm_senderTask = handle;
        return h_comm_senderTask;
    }
}

static void comm_sender_task_entry(void *params)
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
                           portMAX_DELAY);

        if( xResult == pdPASS )
        {
            /* A notification was received.  See which bits were set. */
            if(notifiedValue & EVENT_COMM_SENDER_BOARD_TYPE)
            {
                COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_COMM_MODULE, BBG_LOGGER_MODULE);
                comm_msg.msg_id = MSG_ID_CLIENT_INFO_BOARD_TYPE;
                comm_msg.src_id = TIVA_COMM_MODULE;
                comm_msg.data.nothing = 1;
                strncpy(comm_msg.message,BOARD_TYPE, sizeof(comm_msg.message));
                FILL_CHECKSUM(&comm_msg);
                COMM_SEND(&comm_msg);
                printf("BOARD_TYPE: %s\n",BOARD_TYPE);
            }

            if(notifiedValue & EVENT_COMM_SENDER_CODE_VERSION)
            {
                COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_COMM_MODULE, BBG_LOGGER_MODULE);
                comm_msg.msg_id = MSG_ID_CLIENT_INFO_CODE_VERSION;
                comm_msg.src_id = TIVA_COMM_MODULE;
                comm_msg.data.nothing = 1;
                strncpy(comm_msg.message,CODE_VERSION, sizeof(comm_msg.message));
                FILL_CHECKSUM(&comm_msg);
                COMM_SEND(&comm_msg);
                printf("CODE_VERSION: %s\n",CODE_VERSION);
            }

            if(notifiedValue & EVENT_COMM_SENDER_UID)
            {
                COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_COMM_MODULE, BBG_LOGGER_MODULE);
                comm_msg.msg_id = MSG_ID_CLIENT_INFO_UID;
                comm_msg.src_id = TIVA_COMM_MODULE;
                comm_msg.data.nothing = 1;
                strncpy(comm_msg.message,UID, sizeof(comm_msg.message));
                FILL_CHECKSUM(&comm_msg);
                COMM_SEND(&comm_msg);
                printf("UID: %s\n",UID);
            }

            if(notifiedValue & EVENT_COMM_SENDER_HEARTBEAT)
            {
                COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_HEART_BEAT_MODULE, BBG_LOGGER_MODULE);
                comm_msg.msg_id = MSG_ID_HEARTBEAT;
                comm_msg.src_id = TIVA_COMM_MODULE;
                comm_msg.data.nothing = 1;
                strncpy(comm_msg.message,"HEARTBEAT", sizeof(comm_msg.message));
                FILL_CHECKSUM(&comm_msg);
                COMM_SEND(&comm_msg);
                printf("HEARTBEAT\n");
            }

            if(notifiedValue & EVENT_COMM_SENDER_STATUS)
            {
                COMM_MSG_T comm_msg;
                if(xQueueReceive(h_comm_senderQueue,&comm_msg,xMaxBlockTime))
                {
                    FILL_CHECKSUM(&comm_msg);
                    COMM_SEND(&comm_msg);
                    printf("STATUS: %s\n",comm_msg.message);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_COMM_SENDER_INFO)
            {
                COMM_MSG_T comm_msg;
                if(xQueueReceive(h_comm_senderQueue,&comm_msg,xMaxBlockTime))
                {
                    FILL_CHECKSUM(&comm_msg);
                    COMM_SEND(&comm_msg);
                    printf("INFO: %s\n",comm_msg.message);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }


            if(notifiedValue & EVENT_COMM_SENDER_MSG)
            {
                COMM_MSG_T comm_msg;
                if(xQueueReceive(h_comm_senderQueue,&comm_msg,xMaxBlockTime))
                {
                    comm_msg.msg_id = MSG_ID_MSG;
                    FILL_CHECKSUM(&comm_msg);
                    COMM_SEND(&comm_msg);
                    printf("MSG: %s\n",comm_msg.message);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_COMM_SENDER_ERROR)
            {
                COMM_MSG_T comm_msg;
                if(xQueueReceive(h_comm_senderQueue,&comm_msg,xMaxBlockTime))
                {
                    comm_msg.msg_id = MSG_ID_ERROR;
                    FILL_CHECKSUM(&comm_msg);
                    COMM_SEND(&comm_msg);
                    printf("STATUS: %s\n",comm_msg.message);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_COMM_SENDER_OBJECT_DETECTED)
            {
                COMM_MSG_T comm_msg;
                if(xQueueReceive(h_comm_senderQueue,&comm_msg,xMaxBlockTime))
                {
                    comm_msg.msg_id = MSG_ID_OBJECT_DETECTED;
                    FILL_CHECKSUM(&comm_msg);
                    COMM_SEND(&comm_msg);
                    printf("OBJECT DETECTED: %f cm\n",comm_msg.data.distance_cm);
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

            if(notifiedValue & EVENT_COMM_SENDER_PICTURE)
            {
                COMM_MSG_T comm_msg;
                if(xQueueReceive(h_comm_senderQueue,&comm_msg,xMaxBlockTime))
                {
                    comm_msg.msg_id = MSG_ID_PICTURE;
                    FILL_CHECKSUM(&comm_msg);
                    COMM_SEND(&comm_msg);
                    printf("SENDING PICTURE of size: %u\n",comm_msg.data.camera_packet->length);
                    //Now extract the CAMERA_PACKET and send the raw frame from buffer pointer provided in the camera packet
                    //The camera frame data buffer should exit in the memory
                    CAMERA_PACKET_T *packet;
                    if(comm_msg.data.camera_packet)
                    {
                        packet = comm_msg.data.camera_packet;
                    }
                    else
                    {
                        printf("[ERROR] NULL CAMERA PACKT");
                        packet->length = 0;
                        packet->frame = NULL;
                    }
                    COMM_SENDRAW(packet->frame,packet->length);
                    printf("SUCCESS: SEND PICTURE\n");
                }
                else
                {
                    printf("[Error] Q RECV\n");
                }
            }

        }
//        else
//        {
//            printf("COMM_SENDER NOTIFICATION: TIMEOUT\n");
//        }
    }
}

uint8_t CommSenderTask_init()
{
    /* Creating a Queue required for Logging the msg */
    QueueHandle_t h_comm_senderQ = xQueueCreate(COMM_SENDER_QUEUE_LENGTH, COMM_SENDER_QUEUE_ITEMSIZE);
    setComm_senderQueueHandle(h_comm_senderQ);

    TaskHandle_t h_comm_senderTask;

    /*Initializing the communication interface*/
    COMM_INIT();
//    uint8_t data[32] = {0};
//    NRF_read_data(data, 32);
    /* Create the task*/
    if(xTaskCreate(comm_sender_task_entry, (const portCHAR *)"Comm_sender Task", 128, NULL,
                       tskIDLE_PRIORITY + PRIO_COMM_SENDERTASK, &h_comm_senderTask) != pdTRUE)
    {
        return (1);
    }

    //Setting the comm_sender task handle for future use
    setComm_senderTaskHandle(h_comm_senderTask);
    /* Return the createtask ret value */
    return 0;
}




