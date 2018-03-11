/**
 * @brief 
 * 
 * @file light_sensor_task.c
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
#include "light_sensor_task.h"
#include "error_data.h"


#define MQ_LIGHTTASK_NAME "/lighttask_queue"

static mqd_t lighttask_q;

pthread_mutex_t stateChangeLock;

volatile static DAY_STATE_T isDay;


DAY_STATE_T getLightTask_state()
{
    DAY_STATE_T state;
    pthread_mutex_lock(&stateChangeLock);
    state = isDay;
    pthread_mutex_lock(&stateChangeLock);
    return state;
}

mqd_t getHandle_LightTaskQueue()
{
    return lighttask_q;
}

/**
 * @brief 
 * 
 * @return int 
 */
int light_task_queue_init()
{
    struct mq_attr lighttaskQ_attr = {
        .mq_msgsize = sizeof(LIGHTTASKQ_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_LIGHTTASK_NAME);
    lighttask_q = mq_open(MQ_LIGHTTASK_NAME, O_CREAT | O_RDWR, 0666, &lighttaskQ_attr);

    return lighttask_q;;
}

void light_task_processMsg()
{
    int ret,prio;
    LIGHTTASKQ_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,LIGHT_TASK_ID);
    DEFINE_LOG_STRUCT(logtaskstruct,LT_MSG_LOG,LIGHT_TASK_ID);
    while(1)
    {
        memset(&queueData,0,sizeof(queueData));
        ret = mq_receive(lighttask_q,(char*)&(queueData),sizeof(queueData),&prio);
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        switch(queueData.msgID)
        {
            case(LIGHT_MSG_TASK_STATUS):
                /* Send back task alive response to main task */
                POST_MESSAGE_LOGTASK(&logtaskstruct,INFO "ALIVE STATUS by:%s\n",getTaskIdentfierString(queueData.sourceID));
                POST_MESSAGE_MAINTASK(&maintaskRsp, "Light sensor task Alive");
                break;
            case(LIGHT_MSG_TASK_GET_STATE):
                break;
            case(LIGHT_MSG_TASK_READ_DATA):
                break;
            case(LIGHT_MSG_TASK_WRITE_CMD):
                break;
            case(LIGHT_MSG_TASK_POWERDOWN):
                break;
            case(LIGHT_MSG_TASK_POWERUP):
                break;
            case(LIGHT_MSG_TASK_EXIT):
                break;
            default:
                break;
        }
    }

}

void* light_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "LIGHT TASK STARTED\n");

    int ret = light_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LIGHT TASK INIT%s\n",strerror(errno));
        exit(ERR);
    }

    LOG_STDOUT(INFO "LIGHT TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    /* Process Log queue msg which executes untill the log_task_end flag is set to true*/
    light_task_processMsg();

    return (void*)SUCCESS;
}