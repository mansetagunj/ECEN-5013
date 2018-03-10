/**
 * @brief 
 * 
 * @file logger_task.c
 * @author Gunj Manseta
 * @date 2018-03-09
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

#define MQ_LOGGERTASK_NAME "/loggertask_queue"

/* Keeping the log level to the higest level to log everything. 
    Should be configure at compile time using compile time switch
 */
LOG_LEVEL_T g_loglevel = LOG_ALL;

static mqd_t loggertask_q;

mqd_t getHandle_LoggerTaskQueue()
{
    return loggertask_q;
}

int logger_task_init()
{
    struct mq_attr loggertaskQ_attr = {
        .mq_msgsize = sizeof(LOGGERTASKQ_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    loggertask_q = mq_open(MQ_LOGGERTASK_NAME, O_CREAT | O_RDWR, 0666, &loggertaskQ_attr);

    return loggertask_q;;
}

void logger_task_processMsg()
{
    int ret,prio;
    while(1)
    {
        LOGGERTASKQ_MSG_T queueData = {0};
        ret = mq_receive(loggertask_q,(char*)&(queueData),sizeof(queueData),&prio);
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
        }
        switch(queueData.msgID)
        {
            case(LT_MSG_LOG):
                if(g_loglevel >= queueData.loglevel)
                {
                    LOG_STDOUT(INFO "QUEUE LOG\n");
                }
                break;
            case(LT_MSG_TASK_STATUS):
                /* Send back task alive response to main task */
                break;

            default:
                break;
        }
    }

}


void* logger_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "LOGGER TASK STARTED\n");
    int ret = logger_task_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LOGGER TASK INIT%s\n",strerror(errno));
        exit(ERR);
    }

    LOG_STDOUT(INFO "LOGGER TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    /* Process Log queue msg which executes untill the log_task_end flag is set to true*/
    logger_task_processMsg();

    return (void*)SUCCESS;
}