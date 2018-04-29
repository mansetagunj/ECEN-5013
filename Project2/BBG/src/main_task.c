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
#include "light_sensor_task.h"
#include "temperature_sensor_task.h"
#include "my_signals.h"
#include "posixTimer.h"
#include "common_helper.h"
#include "readConfiguration.h"
#include "comm_recv_task.h"
#include "comm_sender_task.h"
#include "dispatcher_task.h"
#include "BB_Led.h"


#define MQ_MAINTASK_NAME "/maintask_queue"

volatile int timeoutflag;
volatile sig_atomic_t signal_exit;
volatile int aliveStatus[NUM_CHILD_THREADS] = {0};
extern volatile sig_atomic_t comm_recv_task_exit;

void* (*thread_callbacks[NUM_CHILD_THREADS])(void *) =
{
    logger_task_callback,
    temperature_task_callback,
    socket_task_callback,
    light_task_callback,
    comm_recv_task_callback,
    comm_sender_task_callback,
    dispatcher_task_callback
};

extern void install_segfault_signal();

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
    // for(int i = 0; i < NUM_CHILD_THREADS; i++)
    // {
    //     pthread_cancel(pthread_id[i]);
    // }

    signal_exit = 1;
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
        LOG_STDOUT(ERROR "TIMEOUT. App could not be setup in time\n");
        timeoutflag=0;
        delete_timer(*(timer_t*)sig.sival_ptr);
        exit(1);
    }
}

static void timer_handler_aliveStatusCheck(union sigval sig)
{		
    if(!aliveStatus[LOGGER_TASK_ID] && !aliveStatus[LIGHT_TASK_ID] && !aliveStatus[TEMPERATURE_TASK_ID])
    {
        pthread_mutex_lock(&aliveState_lock);
        aliveStatus[LOGGER_TASK_ID]++;
        aliveStatus[LIGHT_TASK_ID]++;
        aliveStatus[TEMPERATURE_TASK_ID]++;
        pthread_mutex_unlock(&aliveState_lock);
        DEFINE_LOG_STRUCT(logstruct,LT_MSG_TASK_STATUS,MAIN_TASK_ID);
        DEFINE_LIGHT_STRUCT(lightstruct,LIGHT_MSG_TASK_STATUS,MAIN_TASK_ID)
        DEFINE_TEMP_STRUCT(tempstruct,TEMP_MSG_TASK_STATUS,MAIN_TASK_ID)
        POST_MESSAGE_LOGTASK(&logstruct,"Send Alive status");
        POST_MESSAGE_LIGHTTASK(&lightstruct);
        POST_MESSAGE_TEMPERATURETASK(&tempstruct);
    }
    else
    {
        LOG_STDOUT(ERROR "One of the task not alive\n");
        stop_timer(*(timer_t*)sig.sival_ptr);
        delete_timer(*(timer_t*)sig.sival_ptr);
        /* Cancelling all the threads for any signals */
        for(int i = 0; i < NUM_CHILD_THREADS && !aliveStatus[i]; i++)
        {
            #ifdef VALUES
            LOG_STDOUT(INFO "Child thread cancelled: %d %s\n",i, getTaskIdentfierString(i));
            #endif
            if(pthread_cancel(pthread_id[i]))
                LOG_STDOUT(INFO "Child thread cancelled failed: %d\n",i);
        }
        #ifdef VALUES
        LOG_STDOUT(INFO "All child thread cancelled\n");
        #endif
        BB_LedON(1);
        /* Signaling main task to quit */
        signal_exit = 1;
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
        recv_timeout.tv_sec += 2;
        ret = mq_timedreceive(maintask_q,(char*)&(queueData),sizeof(queueData),&prio,&recv_timeout);
        if(ERR == ret && ETIMEDOUT == errno)
        {
            continue;
        }
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MAIN TASK:MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        switch(queueData.msgID)
        {
            case(MT_MSG_STATUS_RSP):
                #ifdef STDOUT_ALIVE
                LOG_STDOUT(INFO "ALIVE:%s\n",getTaskIdentfierString(queueData.sourceID));
                #endif
                pthread_mutex_lock(&aliveState_lock);
                aliveStatus[queueData.sourceID]--;
                pthread_mutex_unlock(&aliveState_lock);
                break;
            default:
                LOG_STDOUT(INFO "Invalid Main task queue id\n");
                break;
        }
    }

    LOG_STDOUT(INFO "MAIN TASK GOT EXIT\n");

}

void POST_EXIT_MESSAGE_ALL()
{
    comm_recv_task_exit = 1;
    POST_MESSAGE_COMM_SENDTASK_EXIT("EXIT");
    POST_MESSAGE_DISPATCHERTASK_EXIT("EXIT");
    LOG_STDOUT(WARNING "FIRE IN THE HOLE. EXIT EXIT!\n");
    DEFINE_LOG_STRUCT(logstruct,LT_MSG_TASK_EXIT,MAIN_TASK_ID);
    DEFINE_LIGHT_STRUCT(lightstruct,LIGHT_MSG_TASK_EXIT,MAIN_TASK_ID)
    DEFINE_TEMP_STRUCT(tempstruct,TEMP_MSG_TASK_EXIT,MAIN_TASK_ID)
    POST_MESSAGE_LIGHTTASK_EXIT(&lightstruct);
    POST_MESSAGE_TEMPERATURETASK_EXIT(&tempstruct);
    pthread_cancel(pthread_id[SOCKET_TASK_ID]);    
    POST_MESSAGE_LOGTASK_EXIT(&logstruct,"FIRE IN THE HOLE. EXIT EXIT!");
}

int main_task_entry()
{
    #ifdef SEG_FAULT_HANDLER
    install_segfault_signal();
    #endif
    /* Making the timeout flag true, this should be unset=false within 5 sec else the timer checking the operation 
    will send a kill signal and the app will close
    This is to make sure that the barrier is passed within 5 secs. Extra safety feature which might not be neccessary at all.
    */
    timeoutflag = 1;
    signal_exit = 0;
    int ret = main_task_init();    
    if(-1 == ret)
    {
        LOG_STDOUT(ERROR "MAIN TASK INIT:%s\n",strerror(errno));
        return ret;
    }

    ret = configdata_setup();
    if(ret)
        LOG_STDOUT(ERROR "Could not setup data from config file\n");

    /* Mutex init */
    pthread_mutex_init(&aliveState_lock, NULL);

    /* Registering a timer for 5 sec to check that the barrier is passed */
    timer_t timer_id;
    if(ERR == register_and_start_timer(&timer_id, 20*MICROSEC, 1, timer_handler_setup, &timer_id))
    {
        // LOG_STDOUT(ERROR "Timer Error\n");
        return ERR;
    }

    /* Create a barrier for all the threads + the main task*/
    pthread_barrier_init(&tasks_barrier,NULL,NUM_CHILD_THREADS+1);

    struct sigaction sa;
    /*Registering the signal callback handler*/
	register_signalHandler(&sa,signal_handler, REG_SIG_ALL);

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

    /* Resetting the timeoutflag as we are pass the barrier */
    timeoutflag = 0;

    ret= stop_timer(timer_id);
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "MAIN TASK CANNOT STOP TIMER:%s\n",strerror(errno));
        return ERR;
    }

    ret == delete_timer(timer_id);
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "MAIN TASK CANNOT DELETE TIMER:%s\n",strerror(errno));
    }

    if(ERR == register_and_start_timer(&timer_id, 5*MICROSEC, 0 ,timer_handler_aliveStatusCheck, &timer_id))
    {
        // LOG_STDOUT(ERROR "Timer Start Error\n");
        return ERR;
    }

    send_GET_CLIENT_INFO_UID(TIVA_BOARD1_ID);
    sleep(1);
    send_GET_CLIENT_INFO_UID(XYZ_TIVA_BOARD_ID);

    /* Start message processing which is a blocking call */
    main_task_processMsg();

    delete_timer(timer_id);

    POST_EXIT_MESSAGE_ALL();
    
    for(int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        int retThread = 0;
        //  LOG_STDOUT(INFO "Pthread JOIN:%d\n",i);
        ret = pthread_join(pthread_id[i],(void*)&retThread);
        //LOG_STDOUT(INFO "ThreadID %d: Ret:%d\n",i,retThread);
        if(ret  != 0)
        {
            LOG_STDOUT(ERROR "Pthread join:%d:%s\n",i,strerror(errno));
            return ret;
        }
    }

    pthread_mutex_destroy(&aliveState_lock);
    
    configdata_flush();

    BB_LedOFF(1);

    LOG_STDOUT(INFO "GOODBYE CRUEL WORLD!!!\n");

    return SUCCESS;
}

