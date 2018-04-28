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

char* convert(UART_T uart, unsigned int num, int base);

extern uint32_t g_sysClock;

const uint32_t UART[4] = {UART0_BASE, UART1_BASE, UART2_BASE, UART3_BASE};

void UART_config(UART_T uart, BAUD_RATE_ENUM baudrate)
{

    if(uart == UART_0)
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

    }
    else if(uart == UART_3)
    {
        // Enable the GPIO Peripheral used by the UART.
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        // Enable UART0
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
        while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART3))
        {
        }

        // Configure GPIO Pins for UART mode.
        MAP_GPIOPinConfigure(GPIO_PA4_U3RX);
        MAP_GPIOPinConfigure(GPIO_PA5_U3TX);
        MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);


    }

#ifdef __USE_FREERTOS
    g_pUARTMutex[uart] = xSemaphoreCreateMutex();
#endif
    // Use the system clock oscillator as the UART clock source.
    //UARTClockSourceSet(UART[uart], UART_CLOCK_SYSTEM);
    UARTConfigSetExpClk(UART[uart], g_sysClock, baudrate, UART_CONFIG_NORMAL);
    UARTEnable(UART[uart]);
}

void UART_putRAW(UART_T uart, const uint8_t *data, size_t len)
{
    while(len--)
    {
        UARTCharPut(UART[uart], *data++);
    }
}

size_t UART_getRAW(UART_T uart, uint8_t *data, size_t len)
{
    if(!UARTCharsAvail(UART[uart]))
        return 0;
    size_t i = 0, retrycount = 0;
    while(i<len && retrycount < 8)
    {
        int32_t c = UARTCharGetNonBlocking(UART[uart]);
        if(c == -1)
            retrycount++;
        else
        {
            *(data+i) = c;
            retrycount = 0;
            i++;
        }
    }
    return i;
}

void UART_putstr(UART_T uart, const char *str)
{
    while(*str)
    {
        if(*str == '\n')
            UARTCharPut(UART[uart], '\r');
        UARTCharPut(UART[uart], *str++);
    }
}

void UART_printf(UART_T uart, const char *fmt, ...)
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
            UART_putchar(uart,*p);
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
                UART_putchar(uart,'-');
            }
            sprintf(str,"%f",d);
            UART_putstr(uart, str);
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
            UART_putstr(uart, convert(uart,i,10));
            break;
        case 'o':
            i=va_arg(argp,unsigned int);
            UART_putstr(uart, convert(uart,i,8));
            break;
        case 's':
            s=va_arg(argp,char *);
            UART_putstr(uart, s);
            break;
        case 'u':
            u=va_arg(argp,unsigned int);
            UART_putstr(uart, convert(uart,u,10));
            break;
        case 'x':
            u=va_arg(argp,unsigned int);
            UART_putstr(uart, convert(uart,u,16));
            break;
        case '%':
            UART_putchar(uart,'%');
            break;
        }
    }

    va_end(argp);
}

static char buf0[35];
static char buf1[2];
static char buf2[2];
static char buf3[35];
char * const buff_arr[4] = {buf0, buf1, buf2, buf3};

char* convert(UART_T uart, unsigned int num, int base)
{
    //static char buf[50];
    char *ptr = buff_arr[uart];

    ptr=&(buff_arr[uart])[sizeof(buff_arr[uart])-1];
    *ptr='\0';
    do
    {
        *--ptr="0123456789abcdef"[num%base];
        num/=base;
    }while(num!=0);
    return(ptr);
}
