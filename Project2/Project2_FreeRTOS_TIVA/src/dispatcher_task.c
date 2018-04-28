/*
 * dispatcher_task.c
 *
 *  Created on: 26-Apr-2018
 *      Author: Gunj Manseta
 */

#include "priorities.h"

#include "dispatcher_task.h"
#include "communication_object.h"
#include "comm_sender_task.h"
#include "my_uart.h"

#define DISPATCHER_QUEUE_ITEMSIZE   (sizeof(COMM_MSG_T))
#define DISPATCHER_QUEUE_LENGTH     20


volatile uint8_t dispatcherTaskInitDone = 0;
static QueueHandle_t h_dispatcherQueue;
static TaskHandle_t h_dispatcherTask;

QueueHandle_t DispatcherQueueHandle(QueueHandle_t handle, bool get)
{
    if(get)
        return h_dispatcherQueue;
    else
    {
        h_dispatcherQueue = handle;
        return h_dispatcherQueue;
    }
}

TaskHandle_t DispatcherTaskHandle(TaskHandle_t handle, bool get)
{
    if(get)
        return h_dispatcherTask;
    else
    {
        h_dispatcherTask = handle;
        return h_dispatcherTask;
    }
}

/* Create the entry function */
static void dispatcher_task_entry(void *params)
{
    /* Waits on the notification from comm_recv and deq comm item from the queue, process it depending on the msg id and dst id */
    /* Call function accordingly */
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000);
    BaseType_t xResult;
    COMM_MSG_T comm_msg;
    while(1)
    {
        /* Wait to be notified of an interrupt. */
        xResult = ulTaskNotifyTake( pdFALSE,    /* Using as counting semaphore. */
                                    portMAX_DELAY);

        if( xResult == pdPASS )
        {
            /* A Signal was received.  Dequeue the comm_msg from task queue */
            if(h_dispatcherQueue && xQueueReceive(h_dispatcherQueue,&comm_msg, xMaxBlockTime))
            {
                if(!verifyCheckSum(&comm_msg))
                {
                    printf("Checksum error\n");
                    continue;
                }
                printf("DISPATCHING: %s\n",comm_msg.message);
                /* Process the comm msg. Decide on which parameter do we need to dispatch it*/
                if(comm_msg.dst_id == TIVA_COMM_MODULE)
                {
                    switch(comm_msg.msg_id)
                    {
                    case MSG_ID_GET_CLIENT_INFO_BOARD_TYPE:
                        printf("GET CLIENT INFO BOARD TYPE\n");
                        NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_BOARD_TYPE);
                        break;
                    case MSG_ID_GET_CLIENT_INFO_CODE_VERSION:
                        printf("GET CLIENT INFO CODE VERSION\n");
                        NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_CODE_VERSION);
                        break;
                    case MSG_ID_GET_CLIENT_INFO_UID:
                        printf("GET CLIENT INFO UID\n");
                        NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_UID);
                        break;
                    default:
                        printf("Invalid Msg Id:%d from BOARD ID: %d\n",comm_msg.msg_id,comm_msg.src_brd_id);
                        break;
                    }

                }
                else if(comm_msg.dst_id == TIVA_SENSOR_MODULE)
                {
                    if(comm_msg.msg_id == MSG_ID_GET_SENSOR_STATUS)
                    {
                        COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,BBG_LOGGER_MODULE);
                        comm_msg.msg_id = MSG_ID_SENSOR_STATUS;
                        comm_msg.data.distance_cm = 10.43;
                        COMM_FILL_MSG(comm_msg,"Distance in cm");
                        ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
                    }
                    else if(comm_msg.msg_id == MSG_ID_GET_SENSOR_INFO)
                    {
                        /* This should be done in the sensor task. Here, the queue/task should be filled/notified with object/event.*/
                        COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,BBG_LOGGER_MODULE);
                        comm_msg.msg_id = MSG_ID_SENSOR_INFO;
                        comm_msg.data.distance_cm = 10.43;
                        COMM_FILL_MSG(comm_msg,"Ultrasonic Sensor");
                        ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
                    }
                    else
                    {
                        COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,BBG_LOGGER_MODULE);
                        comm_msg.msg_id = MSG_ID_ERROR;
                        comm_msg.data.distance_cm = 10.43;
                        COMM_FILL_MSG(comm_msg,"Invalid Request");
                        ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
                    }
                }
                else if(comm_msg.dst_id == TIVA_CAMERA_MODULE)
                {
                    if(comm_msg.msg_id == MSG_ID_GET_SENSOR_STATUS)
                    {
                        COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_CAMERA_MODULE,BBG_LOGGER_MODULE);
                        comm_msg.msg_id = MSG_ID_SENSOR_STATUS;
                        comm_msg.data.sensor_value = 0.12;
                        COMM_FILL_MSG(comm_msg,"160x140/jpeg");
                        ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
                    }
                    else if(comm_msg.msg_id == MSG_ID_GET_SENSOR_INFO)
                    {
                        COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_CAMERA_MODULE,BBG_LOGGER_MODULE);
                        comm_msg.msg_id = MSG_ID_SENSOR_INFO;
                        COMM_FILL_MSG(comm_msg,"ArduCAM/jpeg");
                        ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
                    }
                }
                else
                {
                    printf("INVALID MODULE ID\n");
                }
            }
            else
            {
                printf("[Error] Q RECV %s\n",__FUNCTION__);
            }
        }
//        else
//        {
//            printf("DISPATCHER NOTIFICATION: TIMEOUT\n");
//        }
    }
}

uint8_t DispatcherTask_init()
{
    /* Creating a Queue required for getting the comm msg recv from comm recv task */
    QueueHandle_t h_dispatcherQ = xQueueCreate(DISPATCHER_QUEUE_LENGTH, DISPATCHER_QUEUE_ITEMSIZE);
    setDispatcherQueueHandle(h_dispatcherQ);

    TaskHandle_t h_dispatcherTask;

    /* Create the task*/
    if(xTaskCreate(dispatcher_task_entry, (const portCHAR *)"Dispatcher Task", 128, NULL,
                       tskIDLE_PRIORITY + PRIO_DISPATCHERTASK, &h_dispatcherTask) != pdTRUE)
    {
        return (1);
    }

    //Setting the dispatcher task handle for future use
    setDispatcherTaskHandle(h_dispatcherTask);
    /* Return the createtask ret value */
    return 0;

}


