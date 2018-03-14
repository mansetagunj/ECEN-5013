/**
 * @brief 
 * 
 * @file temperature_sensor_task.h
 * @author Gunj Manseta
 * @date 2018-03-11
 */

#ifndef TEMPSENSOR_TASK_H
#define TEMPSENSOR_TASK_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include "common_helper.h"
#include "my_time.h"
#include "error_data.h"
#include "sensor_common_object.h"


typedef enum
{
    TEMP_MSG_TASK_STATUS,
    TEMP_MSG_TASK_GET_TEMP,
    TEMP_MSG_TASK_READ_DATA,
    TEMP_MSG_TASK_WRITE_CMD,
    TEMP_MSG_TASK_POWERDOWN,
    TEMP_MSG_TASK_POWERUP,
    TEMP_MSG_TASK_EXIT,

}TEMPERATURETASKQ_MSGID_T;


typedef struct 
{
    TEMPERATURETASKQ_MSGID_T msgID;
    TASK_IDENTIFIER_T sourceID;
    OBJECT_PACKET_T packet;
    //LOGGER_TASK_MSGDATA_T msgData[LightT_MSG_SIZE];

}TEMPERATURETASKQ_MSG_T;


/**
 * @brief Defines a temp struct with the name given and params with some default values
 * 
 */
#define DEFINE_TEMP_STRUCT(name,msgId,sourceId) \
    TEMPERATURETASKQ_MSG_T name = {      \
        .msgID      = msgId,        \
        .sourceID   = sourceId,     \
        .packet     = {0}           \
    };                               

/**
 * @brief Get the Handle TemperatureTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_TemperatureTaskQueue();

/**
 * @brief 
 * 
 */
#define POST_MESSAGE_TEMPERATURETASK(p_tempstruct)  \
    do{  \
        __POST_MESSAGE_TEMPERATURETASK(getHandle_TemperatureTaskQueue(), p_tempstruct, sizeof(*p_tempstruct)); \
    }while(0)

/**
 * @brief 
 * 
 * @param queue 
 * @param p_tempstruct 
 * @param temp_struct_size 
 */
static inline void __POST_MESSAGE_TEMPERATURETASK(mqd_t queue, const TEMPERATURETASKQ_MSG_T *p_tempstruct, size_t temp_struct_size)
{
    if(-1 == mq_send(queue, (const char*)p_tempstruct, temp_struct_size, 20))
    {
        LOG_STDOUT(ERROR "MQ_SEND:%s\n",strerror(errno));
    }
}

/**
 * @brief Get the TempTask temperature object MT-safe
 * 
 * @return double 
 */
double getTempTask_temperature();

/**
 * @brief Entry point of the temp task thread
 * 
 * @param threadparam 
 * @return void* 
 */
void* temperature_task_callback(void *threadparam);


#endif