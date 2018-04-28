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

volatile uint8_t Sonar_sensorTaskInitDone = 0;

static TaskHandle_t h_Sonar_sensorTask;

static float current_sensor_distance_cm = 0;
static uint8_t object_detected = 0;

TaskHandle_t Sonar_sensorTaskHandle(TaskHandle_t handle, bool get)
{
    if(get)
        return h_Sonar_sensorTask;
    else
    {
        h_Sonar_sensorTask = handle;
        return h_Sonar_sensorTask;
    }
}

static void send_sonarSensorValue()
{
    COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,BBG_LOGGER_MODULE);
    comm_msg.msg_id = MSG_ID_SENSOR_STATUS;
    comm_msg.data.distance_cm = current_sensor_distance_cm;
    COMM_FILL_MSG(comm_msg,"Distance in cm");
    ENQUE_NOTIFY_COMM_SENDER_TASK(comm_msg,EVENT_COMM_SENDER_STATUS);
}

static void send_sonarSensorInfo()
{
    COMM_CREATE_OBJECT(comm_msg,MY_TIVA_BOARD_ID,TIVA_SENSOR_MODULE,BBG_LOGGER_MODULE);
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
//        const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000);
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
//                    printf("Distance: %f\n",current_sensor_distance_cm);
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
                    send_sonarSensorValue();
                }

                if(notifiedValue & EVENT_SONAR_SENSOR_INFO)
                {
                    send_sonarSensorInfo();
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
//    static uint32_t count = 0;
//    if((count%5==0) && object_detected)
//    {
//        object_detected = 0;
//    }

    NOTIFY_SONAR_SENSOR_TASK(EVENT_SONAR_PERIODIC_UPDATEVAL);
//    count++;
}


uint8_t SonarSensorTask_init()
{
    Sonar_sensor_init();
    TaskHandle_t h_Sonar_sensorTask;

    TimerHandle_t periodic_getDistance_timer = xTimerCreate("PERIODIC_GET_DISTANCE", pdMS_TO_TICKS(1000) , pdTRUE,  (void*)0, vPeriodicUpdateTimerCallback);
    DEBUG_ERROR(periodic_getDistance_timer == NULL);


    if(xTaskCreate(sonar_sensor_task_entry, (const portCHAR *)"Sonar Sensor Task", 1024, NULL,
                           tskIDLE_PRIORITY + PRIO_SONAR_SENSOR_TASK, &h_Sonar_sensorTask) != pdTRUE)
    {
        return (1);
    }

    if((xTimerStart(periodic_getDistance_timer, 0)) != pdTRUE)
    {
//        DEBUG_ERROR(1);
        return 1;
    }

    setSonar_sensorTaskHandle(h_Sonar_sensorTask);

    /* Return the createtask ret value */
    return 0;
}
