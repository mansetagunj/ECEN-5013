/*
 * my_tasks.c
 *
 *  Created on: 05-Apr-2018
 *      Author: Gunj Manseta
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "priorities.h"
#include "timers.h"

#include "my_tasks.h"

#define MYTASKSTACKSIZE 128

#define LED_D1_PORT    GPIO_PORTN_BASE
#define LED_D2_PORT    GPIO_PORTN_BASE

#define LED_D1_PIN     GPIO_PIN_1
#define LED_D2_PIN     GPIO_PIN_0

#define NUM_OF_TIMERS   2
TimerHandle_t timer_handles[NUM_OF_TIMERS];

void vTimerCallback(TimerHandle_t h_timer)
{
    //Timer handle for 2hz task
    if(h_timer == timer_handles[0])
    {
        static uint32_t led_val = LED_D1_PIN;

        led_val ^= (LED_D1_PIN);
        GPIOPinWrite(LED_D1_PORT, LED_D1_PIN, led_val);

    }
    //Timer handle for 4hz task
    else if(h_timer == timer_handles[1])
    {
        static uint32_t led_val = LED_D2_PIN;

        led_val ^= (LED_D2_PIN);
        GPIOPinWrite(LED_D2_PORT, LED_D2_PIN, led_val);

    }
}

static void task1_2hz(void *params)
{
    timer_handles[0] = xTimerCreate("Timer2HZ", pdMS_TO_TICKS(500) , pdTRUE,  (void*)0, vTimerCallback);

    if(timer_handles[0] == NULL)
    {
        while(1);
    }

    if((xTimerStart(timer_handles[0], 0)) != pdTRUE)
    {
        while(1);
    }

    vTaskSuspend( NULL );
//    static uint8_t led_val = LED_D1_PIN;
//
//    while(1)
//    {
//        led_val ^= (LED_D1_PIN);
//        GPIOPinWrite(LED_D1_PORT, LED_D1_PIN, led_val);
//        vTaskDelay(pdMS_TO_TICKS(500));
//    }
}

uint8_t Task1_init()
{
    /* Configure the GPIO pins*/
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    MAP_GPIOPinTypeGPIOOutput(LED_D1_PORT, LED_D1_PIN);

    /* Create the task*/
    if(xTaskCreate(task1_2hz, (const portCHAR *)"task_2hz", MYTASKSTACKSIZE, NULL,
                       tskIDLE_PRIORITY + PRIO_MY_TASK1, NULL) != pdTRUE)
    {
        return (1);
    }

    /* Return the createtask ret value */
    return 0;
}

static void task2_4hz(void *params)
{
    timer_handles[1] = xTimerCreate("Timer4HZ", pdMS_TO_TICKS(250) , pdTRUE,  (void*)0, vTimerCallback);

    if(timer_handles[1] == NULL)
    {
        while(1);
    }

    if((xTimerStart(timer_handles[1], 0)) != pdTRUE)
    {
        while(1);
    }

    vTaskSuspend( NULL );
//    static uint8_t led_val = LED_D2_PIN;
//
//    while(1)
//    {
//        led_val ^= (LED_D2_PIN);
//        GPIOPinWrite(LED_D2_PORT, LED_D2_PIN, led_val);
//        vTaskDelay(pdMS_TO_TICKS(250));
//    }
}

uint8_t Task2_init()
{
    /* Configure the GPIO pins*/
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    MAP_GPIOPinTypeGPIOOutput(LED_D2_PORT, LED_D2_PIN);

    /* Create the task*/
    if(xTaskCreate(task2_4hz, (const portCHAR *)"task_4hz", MYTASKSTACKSIZE, NULL,
                       tskIDLE_PRIORITY + PRIO_MY_TASK2, NULL) != pdTRUE)
    {
        return 1;
    }

    /* Return the createtask ret value */
    return 0;
}
