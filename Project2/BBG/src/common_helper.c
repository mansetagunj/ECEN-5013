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
#include "posixTimer.h"


const char* const task_identifier_string[NUM_CHILD_THREADS+1] =
{
    (const char*)"Logger Task",
    (const char*)"Temperature Task",    
    (const char*)"Socket Task",
    (const char*)"Light Task",
    (const char*)"COMM Receiver Task",
    (const char*)"COMM Sender Task",
    (const char*)"Dispatcher Task",
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

int register_and_start_timer(timer_t *timer_id, uint32_t usec, uint8_t oneshot, void (*timer_handler)(union sigval), void *handlerArgs)
{
    if(register_timer(timer_id, timer_handler, timer_id) == -1)
	{
	    LOG_STDOUT("[ERROR] Register Timer\n");
		return ERR;
	}
	// else
	// 	LOG_STDOUT("[INFO] Timer created\n");
	
	if(start_timer(*timer_id , usec, oneshot) == -1)
	{
		LOG_STDOUT("[ERROR] Start Timer\n");
		return ERR;
	}
	// else
	// 	LOG_STDOUT("[INFO] Timer started\n");

}