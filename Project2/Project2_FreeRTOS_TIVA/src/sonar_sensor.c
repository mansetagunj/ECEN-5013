/*
 * sonar_sensor.c
 *
 *  Created on: 28-Apr-2018
 *      Author: Gunj Manseta
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

#include "delay.h"
#include "sonar_sensor.h"
#ifdef DEBUG
#include "my_uart.h"
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#define ULTRASONIC_PORT         GPIO_PORTK_BASE
#define ULTRASONIC_TRIGGER_PIN  GPIO_PIN_0
#define ULTRASONIC_ECHO_PIN     GPIO_PIN_1
#define HIGH(PIN)               PIN
#define LOW(PIN)                0


const float distance_factor = (1.0/120.0);

//start and end for echo pulse
volatile uint32_t pulse_down=0, pulse_up =0;

volatile uint8_t sensor_busy = 0;

void TimerInit();
void echo_interrupt();

void Sonar_sensor_init()
{
    //Configuring the timer required to capture the pulse
    TimerInit();

    //Configure Trigger pin
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    DelayUs(100);
    GPIOPinTypeGPIOOutput(ULTRASONIC_PORT, ULTRASONIC_TRIGGER_PIN);

    //Configure Echo pin
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    DelayUs(10);
    GPIOPinTypeGPIOInput(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN);
    GPIOIntEnable(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN);
    GPIOIntTypeSet(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN,GPIO_BOTH_EDGES);
    GPIOIntRegister(ULTRASONIC_PORT,echo_interrupt);
//    IntMasterEnable();

}

float sonarSensor_getDistance()
{
    uint32_t iteration = 0, retryCount = 0;
    float distance_old = 0, distance = 0;
    while(iteration < 10 && retryCount < 10)
    {
        //Check if the sensor is busy
        if(sensor_busy != 1)
        {
            //Give the required pulse of 10uS to trigger
            GPIOPinWrite(ULTRASONIC_PORT, ULTRASONIC_TRIGGER_PIN, ULTRASONIC_TRIGGER_PIN);
            DelayUs(10);
            GPIOPinWrite(ULTRASONIC_PORT, ULTRASONIC_TRIGGER_PIN, 0);

            /*Wait while the reading is measured. The sensor_busy is cleared by the falling edge of echo pin which
             *is done in the interrupt
             */
            while(sensor_busy != 0);

            //Converts
            pulse_up = pulse_down - pulse_up;
            distance =(float)(distance_factor * pulse_up);
            distance = distance/58;

    #ifdef DEBUG
            printf("[IN]Distance = %f cm \n" ,distance);
    #endif
            (distance_old < distance) ? distance_old = distance : 0;
            iteration++;
        }
        else
        {
            retryCount++;
    #ifdef DEBUG
            printf("Retry: %d\n" ,retryCount);
    #endif
        }
    }

    return (distance > distance_old) ?  distance : distance_old;
}

void TimerInit()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  DelayUs(10);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC_UP);
  //TimerEnable(TIMER2_BASE,TIMER_A);
}

void echo_interrupt()
{
    IntMasterDisable();
    //Clear interrupt flag
    GPIOIntClear(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN);

    /*Echo pulse rising edge*/
    if(GPIOPinRead(ULTRASONIC_PORT, ULTRASONIC_ECHO_PIN) == ULTRASONIC_ECHO_PIN)
    {
        HWREG(TIMER2_BASE + TIMER_O_TAV) = 0; //Loads value 0 into the timer.
//        pulse_up = TimerValueGet(TIMER2_BASE,TIMER_A);
        pulse_up = 0;
        TimerEnable(TIMER2_BASE,TIMER_A);
        sensor_busy=1;
    }
    /*Echo pulse falling edge*/
    else
    {
        pulse_down = TimerValueGet(TIMER2_BASE,TIMER_A);
        TimerDisable(TIMER2_BASE,TIMER_A);
        sensor_busy=0;
    }

    IntMasterEnable();
}
