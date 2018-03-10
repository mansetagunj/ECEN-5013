/**
 * @brief 
 * 
 * @file logger_task.h
 * @author Gunj Manseta
 * @date 2018-03-09
 */

#ifndef LOGGER_TASK_H
#define LOGGER_TASK_H

#include <mqueue.h>
#include "common_helper.h"

#define LT_MSG_SIZE 20

typedef char LOGGER_TASK_MSGDATA_T;

typedef enum
{
    LT_MSG_TASK_STATUS,
    LT_MSG_LOG,
    
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
    LOGGER_TASK_MSGDATA_T msgData[LT_MSG_SIZE];

}LOGGERTASKQ_MSG_T;

/**
 * @brief Get the Handle LoggerTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_LoggerTaskQueue();

/**
 * @brief Entry point of the logger task thread
 * 
 * @param threadparam 
 * @return void* 
 */
void* logger_task_callback(void *threadparam);

#endif