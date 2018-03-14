/**
 * @brief 
 * 
 * @file light_sensor_task.c
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
#include "light_sensor_task.h"
#include "error_data.h"
#include "apds9301_sensor.h"
#include "my_i2c.h"
#include "common_helper.h"


#define MQ_LIGHTTASK_NAME "/lighttask_queue"

#define LUX_THRESHOLD (50)

static mqd_t lighttask_q;

pthread_mutex_t stateChangeLock;

volatile static DAY_STATE_T isDay;


DAY_STATE_T getLightTask_state()
{
    DAY_STATE_T state;

    pthread_mutex_lock(&stateChangeLock);
    state = isDay;
    pthread_mutex_lock(&stateChangeLock);
    return state;
}

static void timer_handler_getAndUpdateState(union sigval sig)
{
    DAY_STATE_T state;

    double lux = APDS9301_getLux();
    if(lux < 0) 
    {
        LOG_STDOUT(ERROR "Lux is negative\n");
        return;
    }
    else 
    {
        LOG_STDOUT(INFO "Lux: %f\n",lux);
    }

    (lux < LUX_THRESHOLD) ? (state = NIGHT) : (state = DAY);
    LOG_STDOUT(INFO "State: %s\n", ((state == DAY)?"DAY":"NIGHT"));
    
    pthread_mutex_lock(&stateChangeLock);
    isDay = state;
    pthread_mutex_lock(&stateChangeLock);
}

mqd_t getHandle_LightTaskQueue()
{
    return lighttask_q;
}

/**
 * @brief 
 * 
 * @return int 
 */
int light_task_queue_init()
{
    struct mq_attr lighttaskQ_attr = {
        .mq_msgsize = sizeof(LIGHTTASKQ_MSG_T),
        .mq_maxmsg = 128,
        .mq_flags = 0,
        .mq_curmsgs = 0
    };

    mq_unlink(MQ_LIGHTTASK_NAME);
    lighttask_q = mq_open(MQ_LIGHTTASK_NAME, O_CREAT | O_RDWR, 0666, &lighttaskQ_attr);

    return lighttask_q;;
}

void light_task_processMsg()
{
    int ret,prio;
    LIGHTTASKQ_MSG_T queueData = {0};
    DEFINE_MAINTASK_STRUCT(maintaskRsp,MT_MSG_STATUS_RSP,LIGHT_TASK_ID);
    DEFINE_LOG_STRUCT(logtaskstruct,LT_MSG_LOG,LIGHT_TASK_ID);
    uint8_t continue_flag = 1;
    while(continue_flag)
    {
        memset(&queueData,0,sizeof(queueData));
        ret = mq_receive(lighttask_q,(char*)&(queueData),sizeof(queueData),&prio);
        if(ERR == ret)
        {
            LOG_STDOUT(ERROR "MQ_RECV:%s\n",strerror(errno));
            continue;
        }
        switch(queueData.msgID)
        {
            case(LIGHT_MSG_TASK_STATUS):
                /* Send back task alive response to main task */
                POST_MESSAGE_LOGTASK(&logtaskstruct,INFO "ALIVE STATUS by:%s\n",getTaskIdentfierString(queueData.sourceID));
                POST_MESSAGE_MAINTASK(&maintaskRsp, "Light sensor task Alive");
                break;
            case(LIGHT_MSG_TASK_GET_STATE):

                // (queueData.packet.reg_value != NULL)? (*queueData.packet.reg_value = getLightTask_state()) : 0;
                // queueData.packet.buffLen = 1;
                // (queueData.packet.is_sync) ? (sem_post(queueData.packet.sync_semaphore)): 0;
                
                break;
            case(LIGHT_MSG_TASK_READ_DATA):
                break;
            case(LIGHT_MSG_TASK_WRITE_CMD):
                break;
            case(LIGHT_MSG_TASK_POWERDOWN):
                APDS9301_powerdown();
                break;
            case(LIGHT_MSG_TASK_POWERUP):
                APDS9301_poweron();
                break;
            case(LIGHT_MSG_TASK_EXIT):
                continue_flag = 0;
                LOG_STDOUT(INFO "Light Task Exit request from:%s\n",getTaskIdentfierString(queueData.sourceID));
                break;
            default:
                break;
        }
    }

}

int light_task_sensorUP(I2C_MASTER_HANDLE_T *i2c)
{
    int ret = 0;
    if(ret = I2Cmaster_Init(i2c) !=0)
    {
        printErrorCode(ret);
        LOG_STDOUT(ERROR "I2C Master init failed\n"); 
    }

    ret = APDS9301_poweron();
    if(ret == 0) LOG_STDOUT(INFO "Sensor powered ON\n");

    ret = APDS9301_test();
    if(ret == 0) {LOG_STDOUT(INFO "[OK] Sensor Test\n");}
    else {LOG_STDOUT(INFO "[FAIL] Sensor Test\n");}

    return ret;
}

int light_task_sensorDOWN(I2C_MASTER_HANDLE_T *i2c)
{
    int ret = 0;
    ret = APDS9301_powerdown();
    if(ret == 0) LOG_STDOUT(INFO "Sensor powered DOWN\n");
    ret = I2Cmaster_Destroy(i2c);
    if(ret != 0)
    {
        printErrorCode(ret);
        LOG_STDOUT(ERROR "I2C Master destroy failed\n"); 
    }

    return ret;
}

void* light_task_callback(void *threadparam)
{
    LOG_STDOUT(INFO "LIGHT TASK STARTED\n");

    int ret = light_task_queue_init();
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LIGHT TASK QUEUE INIT:%s\n",strerror(errno));
        exit(ERR);
    }

    I2C_MASTER_HANDLE_T i2c;
    ret = light_task_sensorUP(&i2c);
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LIGHT TASK SENSOR INIT:%s\n",strerror(errno));
        goto FAIL_EXIT;
        //exit(ERR);
    }

    
    LOG_STDOUT(INFO "[OK] LIGHT TASK INIT COMPLETED\n");
    pthread_barrier_wait(&tasks_barrier);

    /* Registering a timer for 1 sec to update the state of the snesor value by getting the lux value from the sensor*/
    timer_t timer_id;
    if(ERR == register_and_start_timer(&timer_id, 2*MICROSEC, 1, timer_handler_getAndUpdateState, &timer_id))
    {
        LOG_STDOUT(ERROR "Timer Error\n");
        goto FAIL_EXIT;
        //return ERR;
    }

    /* Process Log queue msg which executes untill the log_task_end flag is set to true*/
    light_task_processMsg();

    stop_timer(timer_id);
    delete_timer(timer_id);

FAIL_EXIT:
    mq_close(lighttask_q);
    ret = light_task_sensorDOWN(&i2c);
    if(ERR == ret)
    {
        LOG_STDOUT(ERROR "LIGHT TASK SENSOR END:%s\n",strerror(errno));
        exit(ERR);
    }

    LOG_STDOUT(INFO "Light task exit.\n");
    return (void*)SUCCESS;
}