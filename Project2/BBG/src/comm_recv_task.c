/**
 * @brief 
 * 
 * @file comm_recv_task.c
 * @author Gunj Manseta
 * @date 2018-04-26
 */

#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <errno.h>

#include "error_data.h"
#include "common_helper.h"
#include "comm_recv_task.h"
#include "communication_object.h"
#include "communication_interface.h"

volatile sig_atomic_t comm_recv_task_exit = 0;

/* Create the entry function */
void* comm_recv_task_callback(void *threadparam)
{
    UART_FD_T fd = COMM_INIT();
    int32_t retrycount = 0;
    int32_t checksum_incorrect = 0;
    if(fd < 0)
    {
        /* LOG ERROR */
        return (void*)ERROR;
    }

    LOG_STDOUT(INFO "COMM RECV TASK INIT COMPLETED\n");

    pthread_barrier_wait(&tasks_barrier);

    COMM_MSG_T recv_comm_msg = {0};
    while(!comm_recv_task_exit)
    {
        memset(&recv_comm_msg, 0 , sizeof(recv_comm_msg));
        int32_t ret = comm_recvUART(&recv_comm_msg);
        /* Some error */
        //printf("RET:%d Retry:%d\n",ret,retrycount);
        if(ret == -1)
        {
            /* LOG error */
            LOG_STDOUT(ERROR "COMM RECV\n");
        }
        else if(ret > 0)
        {
            /* Send to dispatcher */
            uint16_t check = getCheckSum(&recv_comm_msg);
            if(check != recv_comm_msg.checksum)
            {
                checksum_incorrect++;
                if(checksum_incorrect > 3)
                {
                    UART_flush();
                    checksum_incorrect = 0;
                }
                continue;
            }
            retrycount = 0;
            LOG_STDOUT(INFO "\n*******\n\
            SRCID:%u, SRC_BRDID:%u, DST_ID:%u, DST_BRDID:%u MSGID:%u\n\
            MSG:%s\n\
            Checksum:%u ?= %u\n********\n",\
            recv_comm_msg.src_id, recv_comm_msg.src_brd_id, recv_comm_msg.dst_id,recv_comm_msg.dst_brd_id,recv_comm_msg.msg_id, recv_comm_msg.message,recv_comm_msg.checksum, check );   
        }
        else
        {
            retrycount++;
            if(retrycount > 55)
            {
                LOG_STDOUT(WARNING "TIVA CONNECTED?\n");
                retrycount = 0;
            }
        }
    }

    COMM_DEINIT(fd);

    LOG_STDOUT(INFO "COMM RECV Task Exit\n");
    return (void*)SUCCESS;

}
/* Blocks on UART recv */
/* Get the msg and enque it to dispatcher task */