/*
 * my_uart.h
 *
 *  Created on: 05-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef MY_UART_H_
#define MY_UART_H_

#ifndef __USE_FREERTOS
#define __USE_FREERTOS
#endif

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

#include "driverlib/uart.h"

typedef enum UART_num
{
    UART_0 = 0,
    UART_1 = 1,
    UART_2 = 2,
    UART_3 = 3,
}UART_T;

/**
* @brief - Available Baud rates for the UART
*/
typedef enum BAUD_RATE
{
    BAUD_921600 = 921600,
    BAUD_460800 = 460800,
    BAUD_230400 = 230400,
    BAUD_115200 = 115200,
    BAUD_38400  = 38400,
    BAUD_57200  = 57200,
    BAUD_9600   = 9600,

}BAUD_RATE_ENUM;

extern const uint32_t UART[4];

#ifdef __USE_FREERTOS

xSemaphoreHandle g_pUARTMutex[4];

/* MACROS are threadsafe */
#define printf(fmt, ...)    xSemaphoreTake(g_pUARTMutex[UART_0], portMAX_DELAY); UART0_printf(fmt, ##__VA_ARGS__); xSemaphoreGive(g_pUARTMutex[UART_0])
#define puts(str)           xSemaphoreTake(g_pUARTMutex[UART_0], portMAX_DELAY); UART0_putstr(str); xSemaphoreGive(g_pUARTMutex[UART_0])
#else

#define printf(fmt, ...)        UART0_printf(fmt, ##__VA_ARGS__)
#define puts(str)               UART0_putstr(str)
#define logger_log(ID, fmt, ...) UART0_printf(fmt, ##__VA_ARGS__)

#endif

#define UART_putchar(uart,ch)   (ch == '\n') ? UARTCharPut(UART[uart], '\r'): 0; UARTCharPut(UART[uart], ch)
#define UART0_putchar(ch)     (ch == '\n') ? UARTCharPut(UART0_BASE, '\r'): 0; UARTCharPut(UART0_BASE, ch)
#define UART3_putchar(ch)     (ch == '\n') ? UARTCharPut(UART3_BASE, '\r'): 0; UARTCharPut(UART3_BASE, ch)

#define UART0_putstr(str)     UART_putstr(UART_0,str)
#define UART3_putstr(str)     UART_putstr(UART_3,str)

#define UART3_putRAW(p_data, len)    UART_putRAW(UART_3, p_data, len)

#define UART0_getRAW(p_data,len)    UART_getRAW(UART_0, p_data, len)
#define UART3_getRAW(p_data,len)    UART_getRAW(UART_3, p_data, len)

#define UART0_config(baudrate)  UART_config(UART_0, baudrate)
#define UART3_config(baudrate)  UART_config(UART_3, baudrate)

#define UART0_printf(fmt, ...)  UART_printf(UART_0,fmt, ##__VA_ARGS__)
#define UART3_printf(fmt, ...)  UART_printf(UART_3,fmt, ##__VA_ARGS__)

void UART_config(UART_T uart, BAUD_RATE_ENUM baudrate);
void UART_putstr(UART_T uart, const char *str);
void UART_printf(UART_T uart, const char *fmt, ...);
void UART_putRAW(UART_T uart, const uint8_t *data, size_t len);
size_t UART_getRAW(UART_T uart, uint8_t *data, size_t len);

#endif /* MY_UART_H_ */
