/**
 * @brief 
 * 
 * @file common_helper.c
 * @author Gunj Manseta
 * @date 2018-03-10
 */

#include "common_helper.h"
#include "main_task.h"
#include "logger_task.h"
#include "light_sensor_task.h"
#include "temperature_sensor_task.h"

//volatile int aliveStatus[NUM_CHILD_THREADS] = {0};

const char* const task_identifier_string[NUM_CHILD_THREADS+1] =
{
    (const char*)"Logger Task",
    (const char*)"Socket Task",
    (const char*)"Light Task",
    (const char*)"Temperature Task",
    (const char*)"Main Task",

};

mqd_t get_queue_handle(TASK_IDENTIFIER_T taskid)
{
    mqd_t queueHandle;
    switch(taskid)
    {
        case(MAIN_TASK_ID):
            queueHandle = getHandle_MainTaskQueue();
            break;
        case(LOGGER_TASK_ID):
            queueHandle = getHandle_LoggerTaskQueue();
            break;
        case(LIGHT_TASK_ID):
            queueHandle = getHandle_LightTaskQueue();
            break;
        case(TEMPERATURE_TASK_ID):
            queueHandle = getHandle_TemperatureTaskQueue();
            break;
        // case(SOCKET_TASK_ID):
        //     queueHandle = getHandle_SocketTaskQueue();
        //     break;
        default:
            queueHandle = 0;
            break;
    }

    return queueHandle;

}