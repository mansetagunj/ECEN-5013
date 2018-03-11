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
#include "posixTimer.h"


#define MQ_MAINTASK_NAME "/maintask_queue"

volatile int timeoutflag;
volatile int signal_exit;

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

    signal_exit = 1;
    /* Cancelling all the threads for any signals */
    for(int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        pthread_cancel(pthread_id[i]);
    }
}

/**
 * @brief Timer handler
 * 
 * @param sigval 
 */
static void timer_handler_setup(union sigval sig)
{		
    if(1 == timeoutflag)
    {
        LOG_STDOUT(ERROR "Past 5 sec\n");
        timeoutflag=0;
        stop_timer(*(timer_t*)sig.sival_ptr);
        delete_timer(*(timer_t*)sig.sival_ptr);
        exit(1);
    }
}

static void timer_handler_aliveStatusCheck(union sigval sig)
{		
    if(aliveStatus[LOGGER_TASK_ID] && aliveStatus[SOCKET_TASK_ID])
    {
        pthread_mutex_lock(&aliveState_lock);
        aliveStatus[LOGGER_TASK_ID]--;
        aliveStatus[SOCKET_TASK_ID]--;
        pthread_mutex_unlock(&aliveState_lock);
    }
    else
    {
        LOG_STDOUT(ERROR "One of the task not alive\n");
        stop_timer(*(timer_t*)sig.sival_ptr);
        delete_timer(*(timer_t*)sig.sival_ptr);
        exit(1);
    }
}

int register_and_start_timer(timer_t *timer_id, void (*timer_handler)(union sigval), void *handlerArgs)
{
    if(register_timer(timer_id, timer_handler, timer_id) == -1)
	{
	    LOG_STDOUT("[ERROR] Register Timer\n");
		return ERR;
	}
	else
		LOG_STDOUT("[INFO] Timer created\n");
	
	if(start_timer(*timer_id , 5000, 0) == -1)
	{
		LOG_STDOUT("[ERROR] Start Timer\n");
		return ERR;
	}
	else
		LOG_STDOUT("[INFO] Timer started\n");

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

    mq_unlink(MQ_MAINTASK_NAME);
    maintask_q = mq_open(MQ_MAINTASK_NAME, O_CREAT | O_RDWR, 0666, &maintaskQ_attr);

    return maintask_q;;
}

void main_task_processMsg()
{
    int ret,prio;
    MAINTASKQ_MSG_T queueData = {0};
    struct timespec recv_timeout = {0};
    while(!signal_exit)
    {
        memset(&queueData,0,sizeof(queueData));
        clock_gettime(CLOCK_REALTIME, &recv_timeout);
        recv_timeout.tv_sec += 3;
        ret = mq_timedreceive(maintask_q,(char*)&(queueData),sizeof(queueData),&prio,&recv_timeout);
        if(ERR == ret && ETIMEDOUT == errno)
        {
            continue;
        }
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        switch(queueData.msgID)
        {
            case(MT_MSG_STATUS_RSP):
                
                break;
            
            default:
                break;
        }
    }


}

int main_task_entry()
{
    /* Making the timeout flag true, this should be unset=false within 5 sec else the timer checking the operation 
    will send a kill signal and the app will close
    This is to make sure that the barrier is passed within 5 secs. Extra safety feature which might not be neccessary at all.
    */
    timeoutflag = 1;
    signal_exit = 0;
    int ret = main_task_init();    
    if(-1 == ret)
    {
        LOG_STDOUT(ERROR "MAIN TASK INIT%s\n",strerror(errno));
        return ret;
    }

    struct sigaction sa;
    /*Registering the signal callback handler*/
	register_signalHandler(&sa,signal_handler, REG_SIG_ALL);

    /* Mutex init */
    pthread_mutex_init(&aliveState_lock, NULL);

    /* Registering a timer for 5 sec to check that the barrier is passed */
    timer_t timer_id;
    register_and_start_timer(&timer_id, timer_handler_setup, &timer_id);

    /* Create a barrier for all the threads + the main task*/
    pthread_barrier_init(&tasks_barrier,NULL,NUM_CHILD_THREADS+1);

    //delete_timer(timer_id);
    //register_and_start_timer(&timer_id, timer_handler_aliveStatusCheck, &timer_id);

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

    stop_timer(timer_id);
    /* Resetting the timeoutflag as we are pas t the barrier */
    timeoutflag = 0;

    /* Start message processing which is a blocking call */
    main_task_processMsg();
    
    for(int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        ret = pthread_join(pthread_id[i],NULL);
        if(ret  != 0)
        {
            LOG_STDOUT(ERROR "Pthread join:%d:%s\n",i,strerror(errno));
            return ret;
        }
    }

    //timer_delete(timer_id);
    pthread_mutex_destroy(&aliveState_lock);

    return SUCCESS;
}

