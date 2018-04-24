/**
 * @brief 
 * 
 * @file main_task.h
 * @author Gunj Manseta
 * @date 2018-03-09
 */

#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include "common_helper.h"
#include "error_data.h"


#define MT_MSG_SIZE 20

typedef char MAINT_TASK_MSGDATA_T;

typedef enum
{
    MT_MSG_STATUS_RSP,
    MT_MSG_INIT_SUCCESS_RSP,

}MAINTASKQ_MSGID_T;

typedef struct 
{
    MAINTASKQ_MSGID_T msgID;
    TASK_IDENTIFIER_T sourceID;
    MAINT_TASK_MSGDATA_T msgData[MT_MSG_SIZE];

}MAINTASKQ_MSG_T;


/**
 * @brief Defines a Main task queue struct with the name given and params with some default values
 * 
 */
#define DEFINE_MAINTASK_STRUCT(name,msgId,sourceId) \
    MAINTASKQ_MSG_T name = {      \
        .msgID      = msgId,        \
        .sourceID   = sourceId,     \
        .msgData    = {0}           \
    };                              

/**
 * @brief 
 * 
 */
#define POST_MESSAGE_MAINTASK(p_maintaskstruct, format, ...)  \
    do{ \
        snprintf((p_maintaskstruct)->msgData,sizeof((p_maintaskstruct)->msgData),format, ##__VA_ARGS__);   \
        __POST_MESSAGE_MAINTASK(getHandle_MainTaskQueue(), p_maintaskstruct, sizeof(*p_maintaskstruct)); \
    }while(0)

/**
 * @brief Post message to the main task using the main task queue handle and giving struct
 * 
 * @param queue 
 * @param main_task_struct 
 * @param maintask_struct_size 
 */
static inline void __POST_MESSAGE_MAINTASK(mqd_t queue, const MAINTASKQ_MSG_T *main_task_struct, size_t maintask_struct_size)
{
    if(-1 == mq_send(queue, (const char*)main_task_struct, maintask_struct_size, 20))
    {
        LOG_STDOUT(ERROR "MAIN:MQ_SEND:%s\n",strerror(errno));
    }
}

/**
 * @brief Get the Handle MainTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_MainTaskQueue();

/**
 * @brief entry point for the main task
 * 
 * @return int 
 */
int main_task_entry();  

#endif