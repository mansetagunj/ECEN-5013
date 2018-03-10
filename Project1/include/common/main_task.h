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
#include "common_helper.h"


//#define DEFINE_MAINTASKQ_MSG(name,msgid,msg) MAINTASKQ_MSG_T name = { .msgId = msgid, memcpy(.msgData,msg)};

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
    MAINT_TASK_MSGDATA_T msgData[MT_MSG_SIZE];

}MAINTASKQ_MSG_T;


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