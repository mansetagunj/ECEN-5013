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

#define COMM_TYPE_NRF
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
#define COMM_SEND(comm_object)      comm_sendNRF(comm_object)
#define NRF_USE_INTERRUPT   (1)
#define NRF_NOTUSE_INTERRUPT   (0)
#else
#define COMM_INIT()                 comm_init_UART(BAUD_921600)     /*comm_init_UART(BAUD_115200)*/
#define COMM_SEND(comm_object)      comm_sendUART(comm_object)
#endif
#endif
#define RX_PIPE 1

//0x54,0x4d,0x52,0x68,0x7C
static uint8_t TXAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
static uint8_t RXAddr[5] = {0xC2,0xC2,0xC2,0xC2,0xC2};

static inline void comm_init_UART(BAUD_RATE_ENUM baudrate)
{
    UART3_config(baudrate);
//    UART3_config(BAUD_921600);
}

static inline void comm_sendUART(COMM_MSG_T comm_object)
{
    UART3_putRAW((uint8_t*)&comm_object, sizeof(comm_object));
    /* This is needed to mark the end of send as the receiving side needs the line termination as the BeagleBone has opened the UART is canonical mode*/
    UART3_putchar('\n');
}

void comm_init_NRF();

void comm_sendNRF_raw(uint8_t *data, uint32_t len);

static inline void comm_sendNRF(COMM_MSG_T comm_object)
{
    NRF_transmit_data((uint8_t*)(&comm_object), sizeof(comm_object), true);
}

#endif /* COMMUNICATION_INTERFACE_H_ */
