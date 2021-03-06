/**
 * @brief 
 * 
 * @file temperature_sensor_task.c
 * @author Gunj Manseta
 * @date 2018-03-11
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
#include "temperature_sensor_task.h"
#include "my_i2c.h"
#include "tmp102_sensor.h"
#include "common_helper.h"


#define MQ_TEMPERATURETASK_NAME "/temperaturetask_queue"

static mqd_t temperaturetask_q;

pthread_mutex_t tempChangeLock;

volatile static float latest_temperature;


float getTempTask_temperature()
{
    float temp;
    pthread_mutex_lock(&tempChangeLock);
    temp = latest_temperature;
    pthread_mutex_unlock(&tempChangeLock);
    return temp;
}

static void timer_handler_getAndUpdateTemperature(union sigval sig)
{
    float temperature;

    DEFINE_LOG_STRUCT(logtaskstruct,LT_MSG_LOG,TEMPERATURE_TASK_ID);

    int ret = TMP102_getTemp_Celcius(&temperature);
    if(ret == 0) 
    {
        #ifdef VALUES
        LOG_STDOUT(INFO "Celcius:%.03f\n",temperature);
        #endif

        #ifdef LOGVALUES
        POST_MESSAGE_LOGTASK(&logtaskstruct,INFO "Celcius:%.03f\n",temperature);
        #endif
    }
    else 
    {
        LOG_STDOUT(ERROR "Temperature Sensor Inactive\n");
        POST_MESSAGE_LOGTASK(&logtaskstruct,ERROR "Temperature Sensor Inactive\n");
        return;
    }
    
    pthread_mutex_lock(&tempChangeLock);
    latest_temperature = temperature;
    pthread_mutex_unlock(&tempChangeLock);
}

mqd_t getHandle_TemperatureTaskQueue()
{
    return temperaturetask_q;
}

/**
 * @brief 
 * 
 * @return int 
 */
int temperature_task_queue_init()
{
    struct mq_attr temperaturetaskQ_attr = {
        .mq_msgsize = sizeof(TEMPERATURETASKQ_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_TEMPERATURETASK_NAME);
    temperaturetask_q = mq_open(MQ_TEMPERATURETASK_NAME, O_CREAT | O_RDWR, 0666, &temperaturetaskQ_attr);

    return temperaturetask_q;;
}

void temperature_task_processMsg()
{
    int ret,prio;
    TEMPERATURETASKQ_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,TEMPERATURE_TASK_ID);
    DEFINE_LOG_STRUCT(logtaskstruct,LT_MSG_LOG,TEMPERATURE_TASK_ID);
    //struct timespec recv_timeout = {0};
    uint8_t continue_flag = 1;
    while(continue_flag)
    {
        memset(&queueData,0,sizeof(queueData));
        // clock_gettime(CLOCK_REALTIME, &recv_timeout);
        // recv_timeout.tv_sec += 3;
        // ret = mq_timedreceive(temperaturetask_q,(char*)&(queueData),sizeof(queueData),&prio, &recv_timeout);
        ret = mq_receive(temperaturetask_q,(char*)&(queueData),sizeof(queueData),&prio);
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
            POST_MESSAGE_LOGTASK(&logtaskstruct,ERROR "MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        switch(queueData.msgID)
        {
            case(TEMP_MSG_TASK_STATUS):
                /* Send back task alive response to main task */
                POST_MESSAGE_LOGTASK(&logtaskstruct,INFO "ALIVE STATUS by:%s\n",getTaskIdentfierString(queueData.sourceID));
                POST_MESSAGE_MAINTASK(&maintaskRsp, "Temperature sensor task Alive");
                break;
            case(TEMP_MSG_TASK_GET_TEMP):
                break;
            case(TEMP_MSG_TASK_READ_DATA):
                break;
            case(TEMP_MSG_TASK_WRITE_CMD):
                break;
            case(TEMP_MSG_TASK_POWERDOWN):
                break;
            case(TEMP_MSG_TASK_POWERUP):
                break;
            case(TEMP_MSG_TASK_EXIT):
                continue_flag = 0;
                LOG_STDOUT(INFO "Temperature Task Exit request from:%s\n",getTaskIdentfierString(queueData.sourceID));
                POST_MESSAGE_LOGTASK(&logtaskstruct,INFO "Temperature Task Exit request from:%s\n",getTaskIdentfierString(queueData.sourceID));
                break;
            default:
                break;
        }
    }

}

/**
 * @brief 
 * 
 * @param i2c 
 * @return int 
 */
int temperature_task_I2Cinit(I2C_MASTER_HANDLE_T *i2c)
{
    int ret = 0;
    if(ret = I2Cmaster_Init(i2c) !=0)
    {
        printErrorCode(ret);
        LOG_STDOUT(ERROR "[FAIL] I2C Master init failed\n"); 
    }

    return ret;
}

/**
 * @brief 
 * 
 * @param i2c 
 * @return int 
 */
int temperature_task_I2Cdeinit(I2C_MASTER_HANDLE_T *i2c)
{
    int ret = 0;
    ret = I2Cmaster_Destroy(i2c);
    if(ret !=0)
    {
        printErrorCode(ret);
        LOG_STDOUT(WARNING "I2C Master destroy failed\n"); 
    }

    return ret;
}

void* temperature_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "TEMPERATURE TASK STARTED\n");

    int ret = temperature_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "TEMPERATURE TASK INIT%s\n",strerror(errno));
        exit(ERR);
    }

    I2C_MASTER_HANDLE_T i2c;
    ret = temperature_task_I2Cinit(&i2c);
    if(ret)
    {
        LOG_STDOUT(ERROR "[FAIL] TEMPERATURE TASK SENSOR INIT:%s\n",strerror(errno));
        goto FAIL_EXIT_SENSOR;
        //exit(ERR);
    }

    if(ret == 0) {LOG_STDOUT(INFO "[OK] Sensor Test\n");}
    else {LOG_STDOUT(INFO "[FAIL] Sensor Test\n");}


    LOG_STDOUT(INFO "TEMPERATURE TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    /* Registering a timer for 2 sec to update the task temp copy by getting the temperature value from the sensor*/
    timer_t timer_id;
    if(ERR == register_and_start_timer(&timer_id, 2*MICROSEC, 0, timer_handler_getAndUpdateTemperature, &timer_id))
    {
        // LOG_STDOUT(ERROR "Timer Error\n");
        goto FAIL_EXIT;
    }

    /* Process Log queue msg which executes untill the log_task_end flag is set to true*/
    temperature_task_processMsg();


    ret = delete_timer(timer_id);
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "TEMPERATURE TASK DELETE TIMER:%s\n",strerror(errno));
        exit(1);
    }

FAIL_EXIT:
    ////* Commented the i2x deint as the light sensor task will deinit the handle. THe handle within the low level i2c is common for a master */
    temperature_task_I2Cdeinit(&i2c);

FAIL_EXIT_SENSOR:
    mq_close(temperaturetask_q);
    LOG_STDOUT(INFO "Temperature task exit.\n");
    return SUCCESS;
}