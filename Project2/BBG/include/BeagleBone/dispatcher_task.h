/**
 * @brief 
 * 
 * @file dispatcher_task.h
 * @author Gunj Manseta
 * @date 2018-04-26
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>

#include "common_helper.h"
#include "communication_object.h"


/**
 * @brief Get the Handle DispatcherTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_DispatcherTaskQueue();


#define POST_MESSAGE_DISPATCHERTASK(p_comm_msg)  \
    do{ \
        __POST_MESSAGE_DISPATCHERTASK(getHandle_DispatcherTaskQueue(), (p_comm_msg), sizeof(*p_comm_msg), 20); \
    }while(0)

#define POST_MESSAGE_DISPATCHERTASK_EXIT(format, ...)  \
    do{ \
        COMM_MSG_T comm_msg;    \
        /*(strlen(format)>0) ? snprintf(comm_msg.message,sizeof(comm_msg.message),format, ##__VA_ARGS__):0; */\
        COMM_OBJECT_MSGID(comm_msg,0xFF); \
        COMM_DST_BOARD_ID(comm_msg,BBG_BOARD_ID); \
        __POST_MESSAGE_DISPATCHERTASK(getHandle_DispatcherTaskQueue(), &comm_msg, sizeof(comm_msg), 20); \
    }while(0)

/**
 * @brief 
 * 
 * @param queue 
 * @param comm_msg 
 * @param comm_msg_size 
 * @param prio 
 */
static inline void __POST_MESSAGE_DISPATCHERTASK(mqd_t queue, const COMM_MSG_T *comm_msg, size_t comm_msg_size, int prio)
{
    if(-1 == mq_send(queue, (const char*)comm_msg, comm_msg_size, prio))
    {
        LOG_STDOUT(ERROR "DISPATCHER:MQ_SEND:%s\n",strerror(errno));
    }
}

/**
 * @brief 
 * 
 * @param threadparam 
 * @return void* 
 */
void* dispatcher_task_callback(void *threadparam);

#endif