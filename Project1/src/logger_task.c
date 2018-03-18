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
#include "readConfiguration.h"

#define LOG_DIR     "./log/"
#define __LOG_PATH(x) LOG_DIR ## x
#define LOG_PATH(x) __LOG_PATH(x)

#define MQ_LOGGERTASK_NAME "/loggertask_queue"


/**
 * @brief USe it carefully as there is not NULL checking of the file stream provided
 * 
 */
#define LT_LOG(fp,format, ...) do{fprintf(fp,"[PID:%d][TID:%ld]",getpid(),syscall(SYS_gettid)); fprintf(fp,format, ##__VA_ARGS__); fflush(fp);}while(0)

/* Keeping the log level to the higest level to log everything. 
    Should be configure at compile time using compile time switch
 */
LOG_LEVEL_T g_loglevel = LOG_ALL;

static mqd_t loggertask_q;

mqd_t getHandle_LoggerTaskQueue()
{
    return loggertask_q;
}

FILE* logger_task_file_init(const char *logFileName)
{
    if(NULL == logFileName)
        return NULL;
    FILE *fp = fopen(logFileName,"r+");
    /* check if the file already exists then close it and save it as old_log */
    if(fp)
    {
        fclose(fp);
        char newFilename[40] = {0};
        snprintf(newFilename,sizeof(newFilename),"%u_%s",(unsigned)time(NULL),logFileName);
        int ret = rename(logFileName, newFilename);
        if(ret)
        {
            LOG_STDOUT(ERROR "Cannot backup old log file\n");
        }
    }
    fp = fopen(logFileName,"w+");
    if(NULL == fp)
    {
        LOG_STDOUT(INFO "Log file created\n");
    }
    return fp;
}

int logger_task_queue_init()
{
    struct mq_attr loggertaskQ_attr = {
        .mq_msgsize = sizeof(LOGGERTASKQ_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_LOGGERTASK_NAME);
    loggertask_q = mq_open(MQ_LOGGERTASK_NAME, O_CREAT | O_RDWR, 0666, &loggertaskQ_attr);

    return loggertask_q;;
}

void logger_task_processMsg(FILE *fp)
{
    int ret,prio;
    LOGGERTASKQ_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,LOGGER_TASK_ID);
    //struct timespec recv_timeout = {0};
    uint8_t continue_flag= 1;
    while(continue_flag)
    {
        memset(&queueData,0,sizeof(queueData));
        // clock_gettime(CLOCK_REALTIME, &recv_timeout);
        // recv_timeout.tv_sec += 3;
        // ret = mq_timedreceive(loggertask_q,(char*)&(queueData),sizeof(queueData),&prio, &recv_timeout);
        ret = mq_receive(loggertask_q,(char*)&(queueData),sizeof(queueData),&prio);
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
        switch(queueData.msgID)
        {
            case(LT_MSG_TASK_EXIT):
                continue_flag = 0;
                LT_LOG(fp,INFO "Logger Task Exit request from:%s\n",getTaskIdentfierString(queueData.sourceID));
                LOG_STDOUT(INFO "Logger Task Exit request from:%s\n",getTaskIdentfierString(queueData.sourceID));
                break;
            case(LT_MSG_LOG):
                if(g_loglevel >= queueData.loglevel)
                {
                    #ifdef STDOUT_LOG
                    LOG_STDOUT(INFO "[%s] Sender:%s\tMsg:%s",queueData.timestamp,getTaskIdentfierString(queueData.sourceID),queueData.msgData);
                    #endif
                    LT_LOG(fp,INFO "[%s] Sender:%s\tMsg:%s",queueData.timestamp,getTaskIdentfierString(queueData.sourceID),queueData.msgData);
                }
                break;
            case(LT_MSG_TASK_STATUS):
                if(MAIN_TASK_ID == queueData.sourceID)
                {
                    /* Send back task alive response to main task */
                    LT_LOG(fp,INFO "[%s] Sender:%s\tMsg:%s",queueData.timestamp,getTaskIdentfierString(queueData.sourceID),queueData.msgData);
                    POST_MESSAGE_MAINTASK(&maintaskRsp, "Logger Alive");
                }
                break;

            default:
            LOG_STDOUT(INFO "INVALID QUEUE LOG ID\n");
                break;
        }
    }

}

void* logger_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "LOGGER TASK STARTED\n");

    char *filename = configdata_getLogpath();
    FILE *fp;
    if(filename)
    {
        fp = logger_task_file_init(filename);
    }
    else
    {
        LOG_STDOUT(WARNING "No filename found from config file\n");
        fp = logger_task_file_init("project1.log");
    }
    
    if(NULL == fp)
    {
        LOG_STDOUT(ERROR "LOGGER TASK LOG FILE INIT FAIL\n");
        exit(ERR);
    }

    int ret = logger_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LOGGER TASK INIT%s\n",strerror(errno));
        exit(ERR);
    }

    LOG_STDOUT(INFO "LOGGER TASK INIT COMPLETED\n");
    LT_LOG(fp,INFO "LOGGER TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    #ifdef VALUES
    LOG_STDOUT(INFO "LOGGER TASK UP and RUNNING\n");
    #endif
    #ifdef LOGVALUES
    LT_LOG(fp,INFO "LOGGER TASK UP and RUNNING\n");
    #endif
    /* Process Log queue msg which executes untill the log_task_end flag is set to true*/
    logger_task_processMsg(fp);

    mq_close(loggertask_q);
    fflush(fp);
    fclose(fp);
    LOG_STDOUT(INFO "Logger Task Exit.\n");
    return (void*)SUCCESS;
}