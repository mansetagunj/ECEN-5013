/**
 * @brief 
 * 
 * @file BB_Led.c
 * @author Gunj Manseta
 * @date 2018-03-10
 */

#include <stdlib.h>
#include <stdio.h>
#include "BB_Led.h"


#define ON "1"
#define OFF "0"


#define LED_COUNT   4
const char *const LEDPATH[LED_COUNT] =
{
    "/sys/class/leds/beaglebone:green:usr0/brightness",
    "/sys/class/leds/beaglebone:green:usr1/brightness",
    "/sys/class/leds/beaglebone:green:usr2/brightness",
    "/sys/class/leds/beaglebone:green:usr3/brightness"
};

int BB_LedON(USER_LED_T lednum)
{
    /* Forcefully using USR LED 1 */
    lednum = 1;
    if(lednum < 4)
    {
        FILE *led_fd = fopen(LEDPATH[lednum], "r+");
        if(led_fd)
        {
            fwrite(ON,1,1,led_fd);
            fclose(led_fd);
            return 0; 
        }   
        else
            return -1;
    }
    else
        return -1;

}

int BB_LedOFF(USER_LED_T lednum)
{
    /* Forcefully using USR LED 1 */
    lednum = 1;
    if(lednum < 4)
    {
        FILE *led_fd = fopen(LEDPATH[lednum], "r+");
        if(led_fd)
        {
            fwrite(OFF,1,1,led_fd);
            fclose(led_fd);
            return 0; 
        }   
        else
            return -1;
    }
    else
        return -1;

}

int BB_LedDefault()
{
    FILE *led_fd = fopen("/sys/class/leds/beaglebone:green:usr0/trigger", "r+");
    if(led_fd)
    {
        fwrite("heartbeat",1,sizeof("heartbeat"),led_fd);
        fclose(led_fd);
        return 0; 
    }   
    else
        return -1;
}