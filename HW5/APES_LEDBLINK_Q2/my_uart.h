/*
 * my_uart.h
 *
 *  Created on: 05-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef MY_UART_H_
#define MY_UART_H_

#include "driverlib/uart.h"

/**
* @brief - Available Baud rates for the UART
*/
typedef enum BAUD_RATE
{
    BAUD_115200 = 115200,
    BAUD_38400  = 38400,
    BAUD_57200  = 57200,
    BAUD_9600   = 9600,

}BAUD_RATE_ENUM;


#define printf(fmt, ...)    UART0_printf(fmt, ##__VA_ARGS__);
#define puts(str)           UART0_putstr(str);

#define UART0_putchar(ch)     (ch == '\n') ? UARTCharPut(UART0_BASE, '\r'): 0; UARTCharPut(UART0_BASE, ch)

void UART0_config(BAUD_RATE_ENUM baudrate);
void UART0_putstr(const char *str);
void UART0_printf(const char *fmt, ...);

#endif /* MY_UART_H_ */
