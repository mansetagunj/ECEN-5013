/*
 * communication_setup.c
 *
 *  Created on: 26-Apr-2018
 *      Author: Gunj Manseta
 */

#include "my_uart.h"
#include "comm_sender_task.h"
#include "comm_receiver_task.h"
#include "dispatcher_task.h"

void CommTask_init()
{
    if(CommSenderTask_init())
    {
        printf("[ERROR] %s\n",__FUNCTION__);
        while(1);
    }

    if(CommReceiverTask_init())
    {
        printf("[ERROR] %s\n",__FUNCTION__);
        while(1);
    }

    if(DispatcherTask_init())
    {
        printf("[ERROR] %s\n",__FUNCTION__);
        while(1);
    }
}
