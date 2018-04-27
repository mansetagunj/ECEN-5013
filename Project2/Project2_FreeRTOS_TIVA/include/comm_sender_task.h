/*
 * comm_sender_task.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef COMM_SENDER_TASK_H_
#define COMM_SENDER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <string.h>
#include "communication_object.h"



#define EVENT_COMM_SENDER_HEARTBEAT      ((0x01)<<0)
#define EVENT_COMM_SENDER_MSG            ((0x01)<<1)
#define EVENT_COMM_SENDER_STATUS         ((0x01)<<2)
#define EVENT_COMM_SENDER_ERROR          ((0x01)<<3)
#define EVENT_COMM_SENDER_INFO           ((0x01)<<4)
#define EVENT_COMM_SENDER_BOARD_TYPE     ((0x01)<<5)
#define EVENT_COMM_SENDER_UID            ((0x01)<<6)
#define EVENT_COMM_SENDER_CODE_VERSION   ((0x01)<<7)
#define EVENT_COMM_SENDER_PICTURE        ((0x01)<<8)
#define EVENT_COMM_SENDER_OBJECT_DETECTED   ((0x01)<<9)

//Handy macros
#define NOTIFY_COMM_OBJECT(EVENT_ID)       xTaskNotify(getComm_senderTaskHandle(),EVENT_ID,eSetBits)

#define COMM_CREATE_OBJECT(name, src_board_id, source_id, dest_id)    COMM_MSG_T name = { .src_brd_id = src_board_id, .src_id = source_id, .dst_id = dest_id }
#define COMM_OBJECT_MSGID(name,msgid)  name.msg_id = msgid
#define FILL_CHECKSUM(p_comm_msg)    do{ (p_comm_msg)->checksum = getCheckSum(p_comm_msg); }while(0)

//TODO: include a mutex lock in here to make the enque and notification atomic. Let's see if needed
#define ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg, EVENT_ID)    \
        do{ \
            if(xQueueSend(getComm_senderQueueHandle(), &comm_msg ,xMaxBlockTime) == pdPASS) \
            {   \
                xTaskNotify(getWorkerTaskHandle(),EVENT_ID,eSetBits);   \
                pdPASS; \
            }   \
            else    \
            {pdFAIL;}  \
        }while(0)

#define getComm_senderQueueHandle()           ({QueueHandle_t h = Comm_senderQueueHandle(NULL,1); h;})
#define setComm_senderQueueHandle(handle)     Comm_senderQueueHandle(handle,0)

#define getComm_senderTaskHandle()           ({TaskHandle_t h = Comm_senderTaskHandle(NULL,1); h;})
#define setComm_senderTaskHandle(handle)     Comm_senderTaskHandle(handle,0)

QueueHandle_t Comm_senderQueueHandle(QueueHandle_t handle, bool get);
TaskHandle_t Comm_senderTaskHandle(TaskHandle_t handle, bool get);

uint8_t CommSenderTask_init();


#endif /* COMM_SENDER_TASK_H_ */
