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
#include <pthread.h>

#define NUM_CHILD_THREADS 2

typedef enum
{
    LOGGER_TASK_ID = 0,
    SOCKET_TASK_ID,
    MAIN_TASK_ID

}TASK_IDENTIFIER_T;

pthread_t pthread_id[NUM_CHILD_THREADS];

mqd_t get_queue_handle(TASK_IDENTIFIER_T taskid);

pthread_barrier_t tasks_barrier;

#endif