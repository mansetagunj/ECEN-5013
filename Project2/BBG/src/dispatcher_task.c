/**
 * @brief 
 * 
 * @file dispatcher_task.c
 * @author Gunj Manseta
 * @date 2018-04-26
 */
#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "main_task.h"
#include "common_helper.h"
#include "logger_task.h"
#include "dispatcher_task.h"
#include "communication_object.h"
#include "communication_interface.h"

#define MQ_DISPATCHER_TASK_NAME "/dispatcher_task_queue"


static mqd_t dispatcher_task_q;

mqd_t getHandle_DispatcherTaskQueue()
{
    return dispatcher_task_q;
}

int dispatcher_task_queue_init()
{
    struct mq_attr dispatcher_taskQ_attr = {
        .mq_msgsize = sizeof(COMM_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_DISPATCHER_TASK_NAME);
    dispatcher_task_q = mq_open(MQ_DISPATCHER_TASK_NAME, O_CREAT | O_RDWR, 0666, &dispatcher_taskQ_attr);

    return dispatcher_task_q;
}

/* from teh socket task */
extern uint8_t gotDistance;
extern COMM_MSG_T socket_comm_msg;

/* Waits on the queue items containing the comm mgs, process it depending on the msg id and dst id */
/* Call function accordingly */
void dispatcher_task_processMsg()
{
    int ret,prio;
    COMM_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,DISPATCHER_TASK_ID);
    DEFINE_LOG_STRUCT(log_struct,LT_MSG_COMM_MSG,DISPATCHER_TASK_ID);
    //struct timespec recv_timeout = {0};
    uint8_t continue_flag= 1;
    while(continue_flag)
    {
        memset(&queueData,0,sizeof(queueData));
        // clock_gettime(CLOCK_REALTIME, &recv_timeout);
        // recv_timeout.tv_sec += 3;
        // ret = mq_timedreceive(loggertask_q,(char*)&(queueData),sizeof(queueData),&prio, &recv_timeout);
        ret = mq_receive(dispatcher_task_q,(char*)&(queueData),sizeof(queueData),&prio);
        // if(ERR == ret && ETIMEDOUT == errno)
        // {
        //     //LOG_STDOUT(ERROR "MQ_RECV TIMEOUT:%s\n",strerror(errno));
        //     continue;
        // }
        if(ERR == ret )
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        if((0xFF) == queueData.msg_id)
        {
            if(queueData.dst_brd_id == BBG_BOARD_ID) 
            {
                continue_flag = 0; 
                continue;
            }
        }

        if(!verifyCheckSum(&queueData))
        {
            LOG_STDOUT(INFO "COMM_MSG Checksum failed\n");
            continue;
        }

        switch(queueData.dst_id)
        {
            case BBG_LOGGER_MODULE:
                POST_COMM_MSG_LOGTASK(&log_struct,queueData);
                break;
            case BBG_COMM_MODULE:
                break;       
            case BBG_SOCKET_MODULE: 
                memcpy(&socket_comm_msg,&queueData,sizeof(socket_comm_msg));
                gotDistance = 1;
                break;
            default:
                LOG_STDOUT(INFO "Invalid msg id\n");
                break;

        }
    }
    gotDistance = 1;
}

/* Create the entry function */
void* dispatcher_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "DISPATCHER STARTED\n");
    int ret = dispatcher_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "DISPATCHER INIT%s\n",strerror(errno));
        //exit(ERR);
        goto EXIT;
    }

    LOG_STDOUT(INFO "DISPATCHER INIT DONE\n");
    pthread_barrier_wait(&tasks_barrier);

    dispatcher_task_processMsg();

    mq_close(dispatcher_task_q);

EXIT:
    LOG_STDOUT(INFO "DISPATCHER TASK EXIT\n");
    return SUCCESS;
}
