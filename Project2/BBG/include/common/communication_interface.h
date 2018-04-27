/*
 * communication_interface.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef COMMUNICATION_INTERFACE_H_
#define COMMUNICATION_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

#include "my_uart.h"
#include "nordic_driver.h"
#include "communication_object.h"

#define NRF_USE_INTERRUPT   (1)
#define NRF_NOTUSE_INTERRUPT   (0)

//#define COMM_TYPE_NRF
//#define RUN_TIME_SWITCH
#ifdef RUN_TIME_SWITCH
volatile uint8_t comm_type_uart = 1;

#define COMM_INIT()                 comm_init_NRF(); comm_init_UART(BAUD_115200)
void COMM_SEND(COMM_MSG_T comm_object)
{
    if(comm_type_uart)
    {
        comm_sendUART(comm_object);
    }
    else
    {
        comm_sendNRF(comm_object);
    }
}
#else
#ifdef COMM_TYPE_NRF
#define COMM_INIT()                 comm_init_NRF()
#define COMM_DEINIT()               comm_deinit_NRF()
#define COMM_SEND(p_comm_object)    comm_sendNRF(p_comm_object)
#define COMM_SENDRAW(packet,len)    comm_sendNRF_raw(packet, len)
#define COMM_RECV(p_comm_object)
#else
#define COMM_INIT()                 comm_init_UART()
//Will be used only on BBG
#define COMM_DEINIT(fd)             comm_deinit_UART(fd)
//#define COMM_INIT()                 comm_init_UART(BAUD_115200)
#define COMM_SEND(p_comm_object)    comm_sendUART(p_comm_object)
#define COMM_SENDRAW(packet,len)    comm_sendUARTRAW(packet,len)
#define COMM_RECV(p_comm_object)    comm_recvUART(p_comm_object)
#endif
#endif
#define RX_PIPE 1

//0x54,0x4d,0x52,0x68,0x7C
static uint8_t TXAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
static uint8_t RXAddr[5] = {0xC2,0xC2,0xC2,0xC2,0xC2};

#ifdef TIVA_BOARD
static inline void comm_init_UART()
{
    UART3_config(BAUD_921600);
}

static inline void comm_deinit_UART(int fd){}


static inline void comm_sendUARTRAW(uint8_t* packet, size_t len)
{
    UART3_putRAW(packet,len);
}

static inline void comm_sendUART(COMM_MSG_T *p_comm_object)
{
    UART3_putRAW((uint8_t*)p_comm_object, sizeof(COMM_MSG_T));
    /* This is needed to mark the end of send as the receiving side needs the line termination as the BeagleBone has opened the UART is canonical mode*/
    //UART3_putchar('\n');
}

static inline size_t comm_recvUART(COMM_MSG_T *comm_object)
{
    return UART3_getRAW((uint8_t*)&comm_object, sizeof(COMM_MSG_T));
}

#else
//For BBG

static inline UART_FD_T comm_init_UART()
{
    return UART_Open(COM_PORT4);
}

static inline void comm_deinit_UART(UART_FD_T fd)
{
    UART_Close(fd);
}

static inline int32_t comm_sendUART(COMM_MSG_T *p_comm_object)
{
    return UART_putRAW((void*)p_comm_object,sizeof(COMM_MSG_T));
}
static inline int32_t comm_sendUARTRAW(COMM_MSG_T * comm_object, size_t len)
{
    return UART_putRAW((void*)comm_object,len);
}

static inline int32_t comm_recvUART(COMM_MSG_T *comm_object)
{
    int32_t available = UART_dataAvailable(100);
    if(available == 1)
    {
        return UART_read((void*)comm_object,sizeof(COMM_MSG_T));
    }
    else
        return available;
}

#endif

//For BBG end

#if 0
void comm_init_NRF();

void comm_sendNRF_raw(uint8_t *data, size_t len);

//TODO:
void comm_recvNRF_raw(uint8_t *data, size_t len);

static inline void comm_sendNRF(COMM_MSG_T *p_comm_object)
{
    NRF_transmit_data((uint8_t*)(p_comm_object), sizeof(COMM_MSG_T), true);
}

#endif

#endif /* COMMUNICATION_INTERFACE_H_ */
