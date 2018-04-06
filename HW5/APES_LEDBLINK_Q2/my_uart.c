/*
 * my_uart.c
 *
 *  Created on: 05-Apr-2018
 *      Author: Gunj Manseta
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "my_uart.h"

#define UART_CONFIG_NORMAL  (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE)

char* convert(unsigned int num, int base);

void UART0_config(BAUD_RATE_ENUM baudrate)
{

    // Enable the GPIO Peripheral used by the UART.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    // Configure GPIO Pins for UART mode.
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the system clock oscillator as the UART clock source.
    //UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

    UARTConfigSetExpClk(UART0_BASE, 120000000, baudrate, UART_CONFIG_NORMAL);

    g_pUARTMutex = xSemaphoreCreateMutex();
    UARTEnable(UART0_BASE);
}

void UART0_putstr(const char *str)
{
    while(*str)
    {
        if(*str == '\n')
            UARTCharPut(UART0_BASE, '\r');
        UARTCharPut(UART0_BASE, *str++);
    }
}




void UART0_printf(const char *fmt, ...)
{
    const char *p;
    int i;
    unsigned int u;
    char *s;
    double d;
    char str[6];
    va_list argp;

    va_start(argp, fmt);

    p=fmt;
    for(p=fmt; *p!='\0';p++)
    {
        if(*p != '%')
        {
            UART0_putchar(*p);
            continue;
        }

        p++;

        switch(*p)
        {
        case 'f' :
            d=va_arg(argp,double);
            if(d<0)
            {
                d=-d;
                UART0_putchar('-');
            }
            sprintf(str,"%f",d);
            UART0_putstr(str);
            break;
        case 'c' :
            i=va_arg(argp,int);
            UART0_putchar(i);
            break;
        case 'd' :
            i=va_arg(argp,int);
            if(i<0)
            {
                i=-i;
                UART0_putchar('-');
            }
            UART0_putstr(convert(i,10));
            break;
        case 'o':
            i=va_arg(argp,unsigned int);
            UART0_putstr(convert(i,8));
            break;
        case 's':
            s=va_arg(argp,char *);
            UART0_putstr(s);
            break;
        case 'u':
            u=va_arg(argp,unsigned int);
            UART0_putstr(convert(u,10));
            break;
        case 'x':
            u=va_arg(argp,unsigned int);
            UART0_putstr(convert(u,16));
            break;
        case '%':
            UART0_putchar('%');
            break;
        }
    }

    va_end(argp);
}


char* convert(unsigned int num, int base)
{
    static char buf[50];
    char *ptr;

    ptr=&buf[sizeof(buf)-1];
    *ptr='\0';
    do
    {
        *--ptr="0123456789abcdef"[num%base];
        num/=base;
    }while(num!=0);
    return(ptr);
}
