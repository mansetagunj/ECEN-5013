/**
 * @brief 
 * 
 * @file main_task.c
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
#include "error_data.h"
#include "logger_task.h"
#include "socket_task.h"
#include "my_signals.h"


#define MQ_MAINTASK_NAME "/maintask_queue"


void* (*thread_callbacks[NUM_CHILD_THREADS])(void *) =
{
    logger_task_callback,
    socket_task_callback
};



static mqd_t maintask_q;

/**
 * @brief  Signal handler for the main task
 *         Should not include stdout log in the handler as it is not thread safe. Find an alternative for this
 *          Maybe use a global atomic type to set the signal type after cancelling all the thereads and check that 
 *          atomic data in the main_task after the join call if a signal occured and then use a stdout log there
 * 
 * @param signal 
 */
static void signal_handler(int signal)
{
	switch (signal)
	{

		case SIGUSR1:
			LOG_STDOUT(SIGNAL "SIGUSR1 signal.\n");
			break;
		case SIGUSR2:
			LOG_STDOUT(SIGNAL "SIGUSR2 signal.\n");
			break;
		case SIGINT:
			LOG_STDOUT(SIGNAL "SIGINT signal.\n");
			break;
		case SIGTERM:
			LOG_STDOUT(SIGNAL "SIGTERM signal.\n");
			break;
		case SIGTSTP:
			LOG_STDOUT(SIGNAL "SIGTSTP signal.\n");
			break;
		default:
			LOG_STDOUT(SIGNAL "Invalid signal.\n");
			break;
	}

    /* Cancelling all the threads for any signals */
    for(int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        pthread_cancel(pthread_id[i]);
    }
}

mqd_t getHandle_MainTaskQueue()
{
    return maintask_q;
}

int main_task_init()
{
    struct mq_attr maintaskQ_attr = {
        .mq_msgsize = sizeof(MAINTASKQ_MSG_T),
        .mq_maxmsg = 32,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    maintask_q = mq_open(MQ_MAINTASK_NAME, O_CREAT | O_RDWR, 0666, &maintaskQ_attr);

    return maintask_q;;
}

void main_task_processMsg()
{



}

int main_task_entry()
{
    int ret = main_task_init();    
    if(-1 == ret)
    {
        LOG_STDOUT(ERROR "MAIN TASK INIT%s\n",strerror(errno));
        return ret;
    }

    struct sigaction sa;
    /*Registering the signal callback handler*/
	register_signalHandler(&sa,signal_handler, REG_SIG_ALL);

    /* Create a barrier for all the threads + the main task*/
    pthread_barrier_init(&tasks_barrier,NULL,NUM_CHILD_THREADS+1);

    /* Create all the child threads */
    for(int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        ret = pthread_create(&pthread_id[i],NULL,thread_callbacks[i],NULL);
        if(ret != 0)
        {
            LOG_STDOUT(ERROR "Pthread create:%d:%s\n",i,strerror(errno));
            return ret;
        }
    }

    LOG_STDOUT(INFO "MAIN TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    /* Start message processing */
    
    for(int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        ret = pthread_join(pthread_id[i],NULL);
        if(ret  != 0)
        {
            LOG_STDOUT(ERROR "Pthread join:%d:%s\n",i,strerror(errno));
            return ret;
        }
    }

    return SUCCESS;
}

