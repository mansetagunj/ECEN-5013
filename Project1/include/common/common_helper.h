/**
 * @brief 
 * 
 * @file common_helper.h
 * @author Gunj Manseta
 * @date 2018-03-09
 */
#ifndef COMMON_HELPER_H
#define COMMON_HELPER_H

#include <mqueue.h>

typedef enum
{
    MAIN_TASK_ID = 0,
    LOGGER_TASK_ID,
    SOCKET_TASK_ID

}TASK_IDENTIFIER_T;

mqd_t get_queue_handle(TASK_IDENTIFIER_T taskid);


#endif