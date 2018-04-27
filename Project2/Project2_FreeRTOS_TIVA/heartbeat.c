/*
 * heartbeat.c
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <comm_sender_task.h>
#include "delay.h"
#include "my_uart.h"

#define NUM_OF_TIMERS   2
static TimerHandle_t timer_handles[NUM_OF_TIMERS];

void vTimerCallback(TimerHandle_t h_timer)
{
    static uint32_t led_val = (GPIO_PIN_1 | GPIO_PIN_0);
    static uint32_t count = 0;
    if(h_timer == timer_handles[0])
    {
        if(count%10 == 0)
        {
            //Notify the comm_sender task with Heartbeat event
            //TODO:Check for return value
            NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_HEARTBEAT);
        }
        if(count%30 == 0)
        {
            NOTIFY_COMM_OBJECT(EVENT_COMM_SENDER_BOARD_TYPE);
        }
        count++;
    }
    //TIMER_LED_HEARTBEAT
    else if(h_timer == timer_handles[1])
    {
        led_val ^= (GPIO_PIN_1 | GPIO_PIN_0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0, led_val);
    }
}

void heartbeat_start(uint32_t log_heartbeat_time_ms, uint32_t led_heartbeat_time_ms)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);

    timer_handles[0] = xTimerCreate("TIMER_LOG_HEARTBEAT", pdMS_TO_TICKS(log_heartbeat_time_ms) , pdTRUE,  (void*)0, vTimerCallback);
    DEBUG_ERROR(timer_handles[0] == NULL);

    timer_handles[1] = xTimerCreate("TIMER_LED_HEARTBEAT", pdMS_TO_TICKS(led_heartbeat_time_ms) , pdTRUE,  (void*)0, vTimerCallback);

    DEBUG_ERROR(timer_handles[1] == NULL);

    if((xTimerStart(timer_handles[0], 0)) != pdTRUE)
    {
        DEBUG_ERROR(1);
    }

    if((xTimerStart(timer_handles[1], 0)) != pdTRUE)
    {
        DEBUG_ERROR(1);
    }
}
