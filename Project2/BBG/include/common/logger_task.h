/**
 * @brief 
 * 
 * @file logger_task.h
 * @author Gunj Manseta
 * @date 2018-03-09
 */

#ifndef LOGGER_TASK_H
#define LOGGER_TASK_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include "common_helper.h"
#include "my_time.h"
#include "error_data.h"
#include "communication_object.h"


#define LT_MSG_SIZE 40

typedef char LOGGER_TASK_MSGDATA_T;

typedef enum
{
    LT_MSG_TASK_STATUS,
    LT_MSG_LOG,
    LT_MSG_TASK_EXIT
    
}LOGGERTASKQ_MSGID_T;

typedef enum
{
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_ALL
    
}LOG_LEVEL_T;

typedef struct 
{
    LOGGERTASKQ_MSGID_T msgID;
    char timestamp[20];
    LOG_LEVEL_T loglevel;
    TASK_IDENTIFIER_T sourceID;
    union{
    LOGGER_TASK_MSGDATA_T msgData[LT_MSG_SIZE];
    COMM_MSG_T comm_msg;
    }msgData;
}LOGGERTASKQ_MSG_T;

/**
 * @brief Defines a Log struct with the name given and params with some default values
 * 
 */
#define DEFINE_LOG_STRUCT(name,msgId,sourceId) \
    LOGGERTASKQ_MSG_T name = {      \
        .msgID      = msgId,        \
        .loglevel   = LOG_ALL,      \
        .sourceID   = sourceId,     \
        .timestamp  = {0},          \
        .msgData.msgData    = {0}           \
    };                               

/**
 * @brief Set the Log loglevel
 * 
 * @param log_msg 
 * @param loglevel 
 */
static inline void set_Log_loglevel(LOGGERTASKQ_MSG_T *log_msg, LOG_LEVEL_T loglevel)   
{
    log_msg->loglevel = loglevel;
}

/**
 * @brief Set the Log currentTimestamp to the currentTime as "sec.nsec"
 * 
 * @param log_msg 
 */
static inline void set_Log_currentTimestamp(LOGGERTASKQ_MSG_T *log_msg)   
{
    get_time_string(log_msg->timestamp,sizeof(log_msg->timestamp));
}

/**
 * @brief Get the Handle LoggerTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_LoggerTaskQueue();

/**
 * @brief 
 * 
 */
#define POST_MESSAGE_LOGTASK(p_logstruct, format, ...)  \
    do{ \
        snprintf((p_logstruct)->msgData.msgData,sizeof((p_logstruct)->msgData.msgData),format, ##__VA_ARGS__);   \
        set_Log_currentTimestamp(p_logstruct); \
        __POST_MESSAGE_LOGTASK(getHandle_LoggerTaskQueue(), p_logstruct, sizeof(*p_logstruct), 20); \
    }while(0)

#define POST_MESSAGE_LOGTASK_EXIT(p_logstruct, format, ...)  \
    do{ \
        snprintf((p_logstruct)->msgData.msgData,sizeof((p_logstruct)->msgData.msgData),format, ##__VA_ARGS__);   \
        set_Log_currentTimestamp(p_logstruct); \
        __POST_MESSAGE_LOGTASK(getHandle_LoggerTaskQueue(), p_logstruct, sizeof(*p_logstruct), 20); \
    }while(0)

/**
 * @brief Post message to the log using the log queue handle and giving log struct
 * 
 * @param queue 
 * @param logstruct 
 * @param log_struct_size 
 */
static inline void __POST_MESSAGE_LOGTASK(mqd_t queue, const LOGGERTASKQ_MSG_T *logstruct, size_t log_struct_size, int prio)
{
    if(-1 == mq_send(queue, (const char*)logstruct, log_struct_size, prio))
    {
        LOG_STDOUT(ERROR "LOGGER:MQ_SEND:%s\n",strerror(errno));
    }
}

/**
 * @brief Entry point of the logger task thread
 * 
 * @param threadparam 
 * @return void* 
 */
void* logger_task_callback(void *threadparam);

#endif