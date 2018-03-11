/**
 * @brief 
 * 
 * @file temperature_sensor_task.c
 * @author Gunj Manseta
 * @date 2018-03-11
 */

#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <errno.h>

#include "main_task.h"
#include "logger_task.h"
#include "error_data.h"
#include "temperature_sensor_task.h"


#define MQ_TEMPERATURETASK_NAME "/temperaturetask_queue"

static mqd_t temperaturetask_q;

pthread_mutex_t tempChangeLock;

volatile static float latest_temperature;


float getTempTask_temperature()
{
    float temp;
    pthread_mutex_lock(&tempChangeLock);
    temp = latest_temperature;
    pthread_mutex_lock(&tempChangeLock);
    return temp;
}

mqd_t getHandle_TemperatureTaskQueue()
{
    return temperaturetask_q;
}

/**
 * @brief 
 * 
 * @return int 
 */
int temperature_task_queue_init()
{
    struct mq_attr temperaturetaskQ_attr = {
        .mq_msgsize = sizeof(TEMPERATURETASKQ_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_TEMPERATURETASK_NAME);
    temperaturetask_q = mq_open(MQ_TEMPERATURETASK_NAME, O_CREAT | O_RDWR, 0666, &temperaturetaskQ_attr);

    return temperaturetask_q;;
}

void temperature_task_processMsg()
{
    int ret,prio;
    TEMPERATURETASKQ_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,TEMPERATURE_TASK_ID);
    DEFINE_LOG_STRUCT(logtaskstruct,LT_MSG_LOG,TEMPERATURE_TASK_ID);
    while(1)
    {
        memset(&queueData,0,sizeof(queueData));
        ret = mq_receive(temperaturetask_q,(char*)&(queueData),sizeof(queueData),&prio);
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        switch(queueData.msgID)
        {
            case(TEMP_MSG_TASK_STATUS):
                /* Send back task alive response to main task */
                POST_MESSAGE_LOGTASK(&logtaskstruct,INFO "ALIVE STATUS by:%s\n",getTaskIdentfierString(queueData.sourceID));
                POST_MESSAGE_MAINTASK(&maintaskRsp, "Temperature sensor task Alive");
                break;
            case(TEMP_MSG_TASK_GET_TEMP):
                break;
            case(TEMP_MSG_TASK_READ_DATA):
                break;
            case(TEMP_MSG_TASK_WRITE_CMD):
                break;
            case(TEMP_MSG_TASK_POWERDOWN):
                break;
            case(TEMP_MSG_TASK_POWERUP):
                break;
            case(TEMP_MSG_TASK_EXIT):
                break;
            default:
                break;
        }
    }

}

void* temperature_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "TEMPERATURE TASK STARTED\n");

    int ret = temperature_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "TEMPERATURE TASK INIT%s\n",strerror(errno));
        exit(ERR);
    }

    LOG_STDOUT(INFO "TEMPERATURE TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    /* Process Log queue msg which executes untill the log_task_end flag is set to true*/
    temperature_task_processMsg();

    return (void*)SUCCESS;
}