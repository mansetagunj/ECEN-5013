/**
 * @brief 
 * 
 * @file light_sensor_task.h
 * @author Gunj Manseta
 * @date 2018-03-11
 */

#ifndef LIGHTSENSOR_TASK_H
#define LIGHTSENSOR_TASK_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include "common_helper.h"
#include "my_time.h"
#include "error_data.h"
#include "sensor_common_object.h"

//#define LightT_MSG_SIZE 40

//typedef char LIGHT_TASK_MSGDATA_T;


typedef enum
{
    DAY = 0,
    NIGHT = 1
}DAY_STATE_T;

typedef enum
{
    LIGHT_MSG_TASK_STATUS,
    LIGHT_MSG_TASK_GET_STATE,
    LIGHT_MSG_TASK_READ_DATA,
    LIGHT_MSG_TASK_WRITE_CMD,
    LIGHT_MSG_TASK_POWERDOWN,
    LIGHT_MSG_TASK_POWERUP,
    LIGHT_MSG_TASK_EXIT,
}LIGHTTASKQ_MSGID_T;


typedef struct 
{
    LIGHTTASKQ_MSGID_T msgID;
    TASK_IDENTIFIER_T sourceID;
    OBJECT_PACKET_T packet;
    //LOGGER_TASK_MSGDATA_T msgData[LightT_MSG_SIZE];

}LIGHTTASKQ_MSG_T;


/**
 * @brief Defines a light struct with the name given and params with some default values
 * 
 */
#define DEFINE_LIGHT_STRUCT(name,msgId,sourceId) \
    LIGHTTASKQ_MSG_T name = {      \
        .msgID      = msgId,        \
        .sourceID   = sourceId,     \
        .packet     = {0}           \
    };                               

/**
 * @brief Get the Handle LightTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_LightTaskQueue();

/**
 * @brief 
 * 
*/
#define POST_MESSAGE_LIGHTTASK(p_lightstruct)  \
    do{ \
        __POST_MESSAGE_LIGHTTASK(getHandle_LightTaskQueue(), p_lightstruct, sizeof(*p_lightstruct),20); \
    }while(0)

/**
 * @brief 
 * 
 */
#define POST_MESSAGE_LIGHTTASK_EXIT(p_lightstruct)  \
    do{ \
        __POST_MESSAGE_LIGHTTASK(getHandle_LightTaskQueue(), p_lightstruct, sizeof(*p_lightstruct),50); \
    }while(0)

/**
 * @brief 
 * 
 * @param queue 
 * @param lightstruct 
 * @param light_struct_size 
 */
static inline void __POST_MESSAGE_LIGHTTASK(mqd_t queue, const LIGHTTASKQ_MSG_T *lightstruct, size_t light_struct_size, int prio)
{
    if(-1 == mq_send(queue, (const char*)lightstruct, light_struct_size, prio))
    {
        LOG_STDOUT(ERROR "LIGHT:MQ_SEND:%s\n",strerror(errno));
    }
}

/**
 * @brief Get the LightTask state object MT-safe
 * 
 * @return DAY_STATE_T 
 */
DAY_STATE_T getLightTask_state();

/**
 * @brief Get the LightTask lux object. MT-safe as it calls a MT-safe function within
 * 
 * @return float 
 */
float getLightTask_lux();

/**
 * @brief Entry point of the light task thread
 * 
 * @param threadparam 
 * @return void* 
 */
void* light_task_callback(void *threadparam);

#endif