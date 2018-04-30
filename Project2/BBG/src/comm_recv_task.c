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
#include <signal.h>

#include "error_data.h"
#include "common_helper.h"
#include "comm_recv_task.h"
#include "communication_object.h"
#include "communication_interface.h"
#include "dispatcher_task.h"

volatile sig_atomic_t comm_recv_task_exit = 0;

static int8_t getFrame();

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
            if(recv_comm_msg.dst_brd_id != BBG_BOARD_ID)
            {
                LOG_STDOUT(INFO "Not my Board ID. I am not touching it.\n");
                continue;
            }
            if(recv_comm_msg.msg_id == MSG_ID_OBJECT_DETECTED)
            {
                if(getFrame())
                {
                    LOG_STDOUT(ERROR "Frame save error.\n");
                }
                else
                {
                    LOG_STDOUT(INFO "Frame saved successfully.\n");
                }

            }
            POST_MESSAGE_DISPATCHERTASK(&recv_comm_msg);
            LOG_STDOUT_COMM(recv_comm_msg);
            /* LOG_STDOUT(INFO "\n*******\n\
            SRCID:%u, SRC_BRDID:%u, DST_ID:%u, DST_BRDID:%u MSGID:%u\n\
            SensorVal: %f MSG:%s\n\
            Checksum:%u ?= %u\n********\n",\
            recv_comm_msg.src_id, recv_comm_msg.src_brd_id, recv_comm_msg.dst_id,recv_comm_msg.dst_brd_id,recv_comm_msg.msg_id,recv_comm_msg.data.distance_cm,recv_comm_msg.message,recv_comm_msg.checksum, check );  
            */ 
        }
        else
        {
            retrycount++;
            if(retrycount > 100)
            {
                LOG_STDOUT(WARNING "TIVA CONNECTED????\n");
                retrycount = 0;
            }
        }
    }

    COMM_DEINIT(fd);

    LOG_STDOUT(INFO "COMM RECV Task Exit\n");
    return (void*)SUCCESS;

}

#define p320    4000
#define p640    9000    

static int8_t getFrame()
{
    /* uint8_t buffer_320p[3600] = {0};
    uint8_t buffer_640p[8500] = {0}; */
    uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t)*p640);
    //uint8_t *buffer = buffer_640p;
    uint8_t temp = 0, temp_last = 0;
    uint32_t len = 0;
    int i = 0;
    uint8_t done = 0;
    uint32_t retry = 0;
    uint8_t getpix = 0;
    uint8_t header = 0;
    //static uint32_t image_count = 0;
    while(1)
    {
        int32_t ret = UART_read((uint8_t*)&getpix,1);
        /* Some error */
        //printf("RET:%d Retry:%d\n",ret,retrycount);
        if(ret == -1)
        {
            /* LOG error */
            LOG_STDOUT(ERROR "Frame Recv\n");
        }
        else if(ret > 0 && (getpix == 0xFF || header == 1) ) 
        {
            if(ret == 1)
            {
                if(getpix == 0xFF)
                    header = 1;
                buffer[i] = getpix;
                //printf("0x%x ",buffer[i]);
                if(temp_last == 0xFF && buffer[i] == 0xD9)
                {
                    LOG_STDOUT(INFO "EOF found\n");
                    done = 1;
                    break;
                }
                temp_last  = buffer[i];
                i++;
            }
        }
        else
        {
            retry++;
            if(retry > 1024)
            {
                //printf("Connected?\n");
                break;
            }
        }
    }
        
    if(done)
    {
        char newFilename[25] = {0};
        snprintf(newFilename,sizeof(newFilename),"%s_%u.%s","image",((unsigned)time(NULL)&0xFFFFFF),"jpg");
        FILE *fp = fopen(newFilename, "wb");
        fwrite(buffer,i,1,fp);
        fclose(fp);
        //image_count++;
        free(buffer);
        return 0;
    }
    free(buffer);
    return 1;
}

