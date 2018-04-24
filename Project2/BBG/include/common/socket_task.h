/**
 * @brief 
 * 
 * @file socket_task.h
 * @author Gunj Manseta
 * @date 2018-03-09
 */

#ifndef SOCKET_TASK_H
#define SOCKET_TASK_H

#include <signal.h>

sig_atomic_t socketTask_continue;

/**
 * @brief 
 * 
 * @param threadparam 
 * @return void* 
 */
void* socket_task_callback(void* threadparam);



#endif