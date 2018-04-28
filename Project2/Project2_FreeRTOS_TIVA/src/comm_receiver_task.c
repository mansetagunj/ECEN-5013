/*
 * comm_receiver_task.c
 *
 *  Created on: 26-Apr-2018
 *      Author: Gunj Manseta
 */

#include "priorities.h"

#include "my_uart.h"
#include "communication_object.h"
#include "communication_interface.h"
#include "comm_receiver_task.h"
#include "dispatcher_task.h"
#include "delay.h"

/* Create the entry task*/
static void comm_receiver_task_entry(void *params)
{
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000);
    /* Blocks on UART recv  OR get the notification from the UART RX ISR*/
    /* Enqueues the recvd msg to the dispatcher task queue*/
    COMM_MSG_T recv_comm_msg;
    while(1)
    {
        memset(&recv_comm_msg, 0 , sizeof(recv_comm_msg));
        size_t ret = COMM_RECV(&recv_comm_msg);
        if(ret > 0)
        {
            if(ret != 32)
            {
                printf("RECV error. Data garbage\n");
            }else
            if(recv_comm_msg.dst_brd_id != MY_TIVA_BOARD_ID)
            {
                printf("Invalid Board Id\n");
            }
            else
            {
                /* Send to dispatcher */
                uint8_t ret = ENQUEUE_NOTIFY_DISPATCHER_TASK(recv_comm_msg);
                if(ret == pdFAIL)
                {
                    printf("DISPATCHER NOTIFY ERROR\n");
                    continue;
                }
                printf("\n*******\n\
                SRCID:%u, SRC_BRDID:%u, DST_ID:%u, MSGID:%u\n\
                MSG:%s\n\
                Checksum:%u ?= %u\n********\n",\
                recv_comm_msg.src_id, recv_comm_msg.src_brd_id, recv_comm_msg.dst_id,recv_comm_msg.msg_id, recv_comm_msg.message,recv_comm_msg.checksum, getCheckSum(&recv_comm_msg));
            }
        }
    }
}


/* Create the init */
uint8_t CommReceiverTask_init()
{
    TaskHandle_t h_comm_receiverTask;

    /*Initializing the communication interface. Not needed. Comm receiver is doing it*/
    //COMM_INIT();
//    uint8_t data[32] = {0};
//    NRF_read_data(data, 32);
    /* Create the task*/
    if(xTaskCreate(comm_receiver_task_entry, (const portCHAR *)"Comm_receiver Task", 128, NULL,
                       tskIDLE_PRIORITY + PRIO_COMM_RECEIVERTASK, &h_comm_receiverTask) != pdTRUE)
    {
        return (1);
    }

    //Setting the comm_receiver task handle for future use
//    setComm_receiverTaskHandle(h_comm_receiverTask);
    /* Return the createtask ret value */
    return 0;
}

