/*
 * dispatcher_task.h
 *
 *  Created on: 26-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef INCLUDE_DISPATCHER_TASK_H_
#define INCLUDE_DISPATCHER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

//TODO: include a mutex lock in here to make the enque and notification atomic. Let's see if needed
#define ENQUEUE_NOTIFY_DISPATCHER_TASK(comm_msg)    \
        ({ \
            uint8_t status = xQueueSend(getDispatcherQueueHandle(), &comm_msg ,xMaxBlockTime);  \
            if(status == pdPASS) \
            {   \
                xTaskNotifyGive(getDispatcherTaskHandle());   \
            }   \
            status; \
        })

#define getDispatcherQueueHandle()           ({QueueHandle_t h = DispatcherQueueHandle(NULL,1); h;})
#define setDispatcherQueueHandle(handle)     DispatcherQueueHandle(handle,0)

#define getDispatcherTaskHandle()           ({TaskHandle_t h = DispatcherTaskHandle(NULL,1); h;})
#define setDispatcherTaskHandle(handle)     DispatcherTaskHandle(handle,0)

QueueHandle_t DispatcherQueueHandle(QueueHandle_t handle, bool get);
TaskHandle_t DispatcherTaskHandle(TaskHandle_t handle, bool get);


uint8_t DispatcherTask_init();


#endif /* INCLUDE_DISPATCHER_TASK_H_ */
