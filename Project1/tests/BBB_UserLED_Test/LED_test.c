/**
 * @brief 
 * 
 * @file LED_test.c
 * @author Gunj Manseta
 * @date 2018-03-10
 */

#include <stdio.h>
#include <unistd.h>
#include "BB_Led.h"


int main()
{
    if(!BB_LedON(1))
        printf("LED ON\n");
    sleep(5);
    if(!BB_LedOFF(1))
        printf("LED OFF\n");
    return 0;
}