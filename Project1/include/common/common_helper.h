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

#include "posixTimer.h"

#define NUM_CHILD_THREADS 4

typedef enum
{
    SOCKET_TASK_ID = 0,
    LIGHT_TASK_ID,
    TEMPERATURE_TASK_ID,
    LOGGER_TASK_ID,
    MAIN_TASK_ID

}TASK_IDENTIFIER_T;

volatile int aliveStatus[NUM_CHILD_THREADS];

pthread_mutex_t aliveState_lock;


pthread_t pthread_id[NUM_CHILD_THREADS];

mqd_t get_queue_handle(TASK_IDENTIFIER_T taskid);

pthread_barrier_t tasks_barrier;


extern const char* const task_identifier_string[NUM_CHILD_THREADS+1];

/**
 * @brief Get the Task Identfier String
 * 
 * @param taskid 
 * @return const char* 
 */
static inline const char* getTaskIdentfierString(TASK_IDENTIFIER_T taskid)
{
    return task_identifier_string[taskid];
}


/**
 * @brief Registers a timer, addigns the handler and starts it
 * 
 * @param timer_id 
 * @param usec 
 * @param oneshot 
 * @param timer_handler 
 * @param handlerArgs 
 * @return int 
 */
int register_and_start_timer(timer_t *timer_id, uint32_t usec, uint8_t oneshot, void (*timer_handler)(union sigval), void *handlerArgs);


#endif