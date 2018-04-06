/*
 * worker_task.h
 *
 *  Created on: 06-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef WORKER_TASK_H_
#define WORKER_TASK_H_

#include "queue.h"

#define TOGGLE_LED  (0x01)
#define LOG_STRING  (0x02)

#define getWorkerQueueHandle()           ({QueueHandle_t h = workerQueueHandle(NULL,1); h;})
#define setWorkerQueueHandle(handle)     workerQueueHandle(handle,0)

#define getWorkerTaskHandle()           ({TaskHandle_t h = workerTaskHandle(NULL,1); h;})
#define setWorkerTaskHandle(handle)     workerTaskHandle(handle,0)

typedef struct
{
    TickType_t tickcount;
    char msg[25];
}WORKER_NOTIFY_MSG_T;

QueueHandle_t workerQueueHandle(QueueHandle_t handle, bool get);
TaskHandle_t workerTaskHandle(TaskHandle_t handle, bool get);
uint8_t WorkerTask_init();

#endif /* WORKER_TASK_H_ */
