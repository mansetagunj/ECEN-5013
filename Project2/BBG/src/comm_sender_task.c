/**
 * @brief 
 * 
 * @file comm_sender_task.c
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
#include "comm_sender_task.h"
#include "communication_object.h"
#include "communication_interface.h"

#define MQ_COMM_SENDER_TASK_NAME "/comm_sender_task_queue"


static mqd_t comm_sender_task_q;

mqd_t getHandle_CommSenderTaskQueue()
{
    return comm_sender_task_q;
}

/* Create a queue to get the request and process that. According to the msg id, create the packet and send it */
int comm_sender_task_queue_init()
{
    struct mq_attr comm_sender_taskQ_attr = {
        .mq_msgsize = sizeof(COMM_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_COMM_SENDER_TASK_NAME);
    comm_sender_task_q = mq_open(MQ_COMM_SENDER_TASK_NAME, O_CREAT | O_RDWR, 0666, &comm_sender_taskQ_attr);

    return comm_sender_task_q;;
}

void comm_sender_task_processMsg()
{
    int ret,prio;
    COMM_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,LOGGER_TASK_ID);
    //struct timespec recv_timeout = {0};
    uint8_t continue_flag= 1;
    while(continue_flag)
    {
        memset(&queueData,0,sizeof(queueData));
        // clock_gettime(CLOCK_REALTIME, &recv_timeout);
        // recv_timeout.tv_sec += 3;
        // ret = mq_timedreceive(loggertask_q,(char*)&(queueData),sizeof(queueData),&prio, &recv_timeout);
        ret = mq_receive(comm_sender_task_q,(char*)&(queueData),sizeof(queueData),&prio);
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
        switch(queueData.msg_id)
        {
            case 0xFF:
                (queueData.dst_brd_id == BBG_BOARD_ID) ? continue_flag = 0 :0;
                break;
            case MSG_ID_GET_SENSOR_STATUS:
                if(sizeof(queueData) != COMM_SEND(&queueData))
                    LOG_STDOUT(WARNING "COMM SEND NO HOST\n");
                break;
            case MSG_ID_GET_SENSOR_INFO:
                if(sizeof(queueData) != COMM_SEND(&queueData))
                    LOG_STDOUT(WARNING "COMM SEND NO HOST\n");
                break;
            case MSG_ID_GET_CLIENT_INFO_BOARD_TYPE:
                break;
                if(sizeof(queueData) != COMM_SEND(&queueData))
                    LOG_STDOUT(WARNING "COMM SEND NO HOST\n");
            case MSG_ID_GET_CLIENT_INFO_UID:
                if(sizeof(queueData) != COMM_SEND(&queueData))
                    LOG_STDOUT(WARNING "COMM SEND NO HOST\n");
                break;
            case MSG_ID_GET_CLIENT_INFO_CODE_VERSION:
                if(sizeof(queueData) != COMM_SEND(&queueData))
                    LOG_STDOUT(WARNING "COMM SEND NO HOST\n");
                break;
            default:
                LOG_STDOUT(INFO "Invalid msg id\n");

        }
    }
}

/* Create the entry function */
void* comm_sender_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "COMM SENDER STARTED\n");
    int ret = comm_sender_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LOGGER TASK INIT%s\n",strerror(errno));
        //exit(ERR);
        goto EXIT;
    }

    UART_FD_T fd = COMM_INIT();
    if(fd < 0)
        goto EXIT_COMM;

    LOG_STDOUT(INFO "COMM SENDER INIT DONE\n");
    pthread_barrier_wait(&tasks_barrier);

    comm_sender_task_processMsg();

    COMM_DEINIT(fd);

EXIT_COMM:
    mq_close(comm_sender_task_q);
EXIT:
    LOG_STDOUT(INFO "COMM SENDER TASK EXIT\n");
    return SUCCESS;

}