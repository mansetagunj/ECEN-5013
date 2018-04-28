/*
 * ultrasonic_sensor_task.h
 *
 *  Created on: 27-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef INCLUDE_SONAR_SENSOR_TASK_H_
#define INCLUDE_SONAR_SENSOR_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include <string.h>

#define EVENT_SONAR_PERIODIC_UPDATEVAL   ((0x01)<<0)
#define EVENT_SONAR_REQUEST_GETVAL       ((0x01)<<1)
#define EVENT_SONAR_SENSOR_INFO          ((0x01)<<2)


//Handy macros
#define NOTIFY_SONAR_SENSOR_TASK(EVENT_ID)    xTaskNotify(getSonar_sensorTaskHandle(),EVENT_ID,eSetBits)

#define getSonar_sensorTaskHandle()           ({TaskHandle_t h = Sonar_sensorTaskHandle(NULL,1); h;})
#define setSonar_sensorTaskHandle(handle)     Sonar_sensorTaskHandle(handle,0)

TaskHandle_t Sonar_sensorTaskHandle(TaskHandle_t handle, bool get);

uint8_t SonarSensorTask_init();

#endif /* INCLUDE_SONAR_SENSOR_TASK_H_ */
