/**
 * @brief 
 * 
 * @file my_uart.h
 * @author Gunj Manseta
 * @date 2018-04-23
 */

#ifndef MYUART_H
#define MYUART_H

typedef enum
{
    COM_PORT1 = 1,  //"/dev/ttyS1"
    COM_PORT2,      //"/dev/ttyS2"
    COM_PORT3,      //"/dev/ttyS3"
    COM_PORT4,      //"/dev/ttyS4"
}COM_PORT;

typedef int UART_FD_T;

/**
 * @brief 
 * 
 * @return UART_FD_T 
 */
UART_FD_T UART_Open(COM_PORT com_port);  

/**
 * @brief 
 * 
 * @param fd 
 */
void UART_Close(UART_FD_T fd);


/**
 * @brief 
 * 
 * @param c 
 * @return int32_t 
 */
int32_t UART_putchar(char c);


/**
 * @brief 
 * 
 * @param object 
 * @param len 
 * @return int32_t 
 */
int32_t UART_putRAW(void *object, size_t len);

/**
 * @brief 
 * 
 * @param str 
 * @return int32_t 
 */
int32_t UART_putstr(const char* str);

/**
 * @brief 
 * 
 * @param object 
 * @param len 
 * @return int32_t 
 */
int32_t UART_read(void *object, size_t len);


#endif