/*
 * logger_task.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef LOGGER_TASK_H_
#define LOGGER_TASK_H_

#include "queue.h"
#include <string.h>
#include "communication_object.h"

//TODO: Move the below constant strings to somewhere suitable
const char * const BOARD_TYPE = "TM4C1294XL";
const char * const OS = "FreeRTOS";
const char * const CODE_VERSION = "v1.0";
const char * const UID = "guma9188";
const uint32_t BOARD_UID = (0xA5U<<24);



#define EVENT_LOG_HEARTBEAT      (0x01)
#define EVENT_LOG_MSG            (0x02)
#define EVENT_LOG_STATUS         (0x04)
#define EVENT_LOG_ERROR          (0x08)
#define EVENT_LOG_INFO           (0x10)
#define EVENT_LOG_BOARD_TYPE     (0x20)
#define EVENT_LOG_UID            (0x40)
#define EVENT_LOG_CODE_VERSION   (0x80)

typedef struct
{
    char msg[26];
}LOG_QUEUE_MSG_T;

//Handy macros
#define NOTIFY_LOG_TASK(EVENT_ID)       xTaskNotify(getLoggerTaskHandle(),EVENT_ID,eSetBits);

//TODO: include a mutex lock in here to make the enque and notification atomic
#define ENQUE_NOTIFY_LOG_TASK(string_msg, EVENT_ID)    \
        do{ \
            LOG_QUEUE_MSG_T q_msg;                      \
            strncpy(q_msg.msg,msg,sizeof(q_msg.msg));   \
            if(xQueueSend(getLoggerQueueHandle(), &q_msg ,xMaxBlockTime) == pdPASS) \
            {   \
                xTaskNotify(getWorkerTaskHandle(),EVENT_ID,eSetBits);   \
            }   \
            else    \
            {   \
                while(1);   \
            }   \
        }while(0)

#define getLoggerQueueHandle()           ({QueueHandle_t h = LoggerQueueHandle(NULL,1); h;})
#define setLoggerQueueHandle(handle)     LoggerQueueHandle(handle,0)

#define getLoggerTaskHandle()           ({TaskHandle_t h = LoggerTaskHandle(NULL,1); h;})
#define setLoggerTaskHandle(handle)     LoggerTaskHandle(handle,0)

QueueHandle_t loggerQueueHandle(QueueHandle_t handle, bool get);
TaskHandle_t loggerTaskHandle(TaskHandle_t handle, bool get);

uint8_t LoggerTask_init();


#endif /* LOGGER_TASK_H_ */
