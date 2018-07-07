/*
 * sonar_sensor_task.c
 *
 *  Created on: 27-Apr-2018
 *      Author: Gunj Manseta
 */

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "priorities.h"
#include <limits.h>

#include "sonar_sensor_task.h"
#include "comm_sender_task.h"
#include "communication_object.h"
#include "sonar_sensor.h"
#include "delay.h"

#define DISTANCE_THRESHOLD_CM   10

#define SONAR_SENSOR_QUEUE_ITEMSIZE   (sizeof(COMM_MSG_T))
#define SONAR_SENSOR_QUEUE_LENGTH     20

volatile uint8_t TaskInitDone = 0;

static TaskHandle_t h_sonar_sensorTask;
static QueueHandle_t h_sonar_sensorQueue;

static float current_sensor_distance_cm = 0;
static uint8_t object_detected = 0;

QueueHandle_t Sonar_sensorQueueHandle(QueueHandle_t handle, bool get)
{
    if(get)
        return h_sonar_sensorQueue;
    else
    {
        h_sonar_sensorQueue = handle;
        return h_sonar_sensorQueue;
    }
}

TaskHandle_t Sonar_sensorTaskHandle(TaskHandle_t handle, bool get)
{
    if(get)
        return h_sonar_sensorTask;
    else
    {
        h_sonar_sensorTask = handle;
        return h_sonar_sensorTask;
    }
}

static void send_sonarSensorValue(uint8_t dst_board_id, uint8_t dst_module_id)
{
    COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,dst_module_id);
    comm_msg.dst_brd_id = dst_board_id;
    comm_msg.msg_id = MSG_ID_SENSOR_STATUS;
    comm_msg.data.distance_cm = current_sensor_distance_cm;
    COMM_FILL_MSG(comm_msg,"Distance in cm");
    ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
}

static void send_sonarSensorInfo(uint8_t dst_board_id, uint8_t dst_module_id)
{
    COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,dst_module_id);
    comm_msg.dst_brd_id = dst_board_id;
    comm_msg.msg_id = MSG_ID_SENSOR_INFO;
    comm_msg.data.distance_cm = current_sensor_distance_cm;
    COMM_FILL_MSG(comm_msg,"Sonar/unit:cm/1s");
    ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_INFO);
}

static void send_sonarObjectDetected()
{
    COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,BBG_LOGGER_MODULE);
    comm_msg.msg_id = MSG_ID_OBJECT_DETECTED;
    comm_msg.data.distance_cm = current_sensor_distance_cm;
    COMM_FILL_MSG(comm_msg,"Sonar/Th:10cm");
    ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_OBJECT_DETECTED);
}


/* Create the entry task*/
static void sonar_sensor_task_entry(void *params)
{
        const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500);
        BaseType_t xResult;
        uint32_t notifiedValue = 0;
        while(1)
        {
            /* Wait to be notified of an interrupt. */
            xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
                              ULONG_MAX,        /* Clear all bits on exit. */
                              &notifiedValue, /* Stores the notified value. */
                              portMAX_DELAY);

            if( xResult == pdPASS )
            {
                if(notifiedValue & EVENT_SONAR_PERIODIC_UPDATEVAL)
                {
                    //Perform Measurement
                    current_sensor_distance_cm = sonarSensor_getDistance();
#ifdef DEBUG
                    //printf("Sonar Sensor Distance: %f\n",current_sensor_distance_cm);
#endif
                    (object_detected%5 == 0)  ? object_detected = 0 : object_detected++;
                    if(!object_detected && (current_sensor_distance_cm > 0) && (current_sensor_distance_cm < DISTANCE_THRESHOLD_CM))
                    {
                        //Notify object detected
                        send_sonarObjectDetected();
                        object_detected = 1;
                    }
                }

                if(notifiedValue & EVENT_SONAR_REQUEST_GETVAL)
                {
                    COMM_MSG_T comm_msg = {0};
                    if(h_sonar_sensorQueue && xQueueReceive(h_sonar_sensorQueue,&comm_msg, xMaxBlockTime))
                    {
                        send_sonarSensorValue(comm_msg.src_brd_id, comm_msg.src_id);
                    }
                    else
                    {
                        printf("SONAR QUEUE Timeout\n");
                    }

                }


                if(notifiedValue & EVENT_SONAR_SENSOR_INFO)
                {
                    COMM_MSG_T comm_msg = {0};
                    if(h_sonar_sensorQueue && xQueueReceive(h_sonar_sensorQueue,&comm_msg, xMaxBlockTime))
                    {
                        send_sonarSensorInfo(comm_msg.src_brd_id, comm_msg.src_id);
                    }
                    else
                    {
                        printf("SONAR QUEUE Timeout\n");
                    }
                }
            }
//            else
//            {
//                printf("SENSOR NOTIFICATION: TIMEOUT\n");
//            }
        }


}

void vPeriodicUpdateTimerCallback(TimerHandle_t h_timer)
{
    NOTIFY_SONAR_SENSOR_TASK(EVENT_SONAR_PERIODIC_UPDATEVAL);
}


uint8_t SonarSensorTask_init()
{
    Sonar_sensor_init();
    TaskHandle_t h_Task;
    QueueHandle_t h_sonar_sensorQ = xQueueCreate(SONAR_SENSOR_QUEUE_LENGTH, SONAR_SENSOR_QUEUE_ITEMSIZE);
    setSonar_sensorQueueHandle(h_sonar_sensorQ);


    TimerHandle_t periodic_getDistance_timer = xTimerCreate("PERIODIC_GET_DISTANCE", pdMS_TO_TICKS(500) , pdTRUE,  (void*)0, vPeriodicUpdateTimerCallback);
    DEBUG_ERROR(periodic_getDistance_timer == NULL);


    if(xTaskCreate(sonar_sensor_task_entry, (const portCHAR *)"Sonar Sensor Task", 1024, NULL,
                           tskIDLE_PRIORITY + PRIO_SONAR_SENSOR_TASK, &h_Task) != pdTRUE)
    {
        return (1);
    }

    if((xTimerStart(periodic_getDistance_timer, 0)) != pdTRUE)
    {
//        DEBUG_ERROR(1);
        return 1;
    }

    setSonar_sensorTaskHandle(h_Task);

    /* Return the createtask ret value */
    return 0;
}
