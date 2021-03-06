/*
 * communication_interface.c
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#if 1
#include "communication_interface.h"

/* NRF COMM FUNCTIONS*/
void my_NRF_IntHandler()
{
}

volatile uint8_t count = 0;
int8_t comm_init_NRF()
{
    if(count)
    {
        count++;
        return 0;
    }
    int8_t status = NRF_moduleInit(NRF_USE_INTERRUPT, my_NRF_IntHandler);
    if(status == -1)
        return status;
    NRF_moduleSetup(NRF_DR_1Mbps, NRF_PW_MED);
    NRF_openReadPipe(1, RXAddr, sizeof(COMM_MSG_T)>32 ? 32 : sizeof(COMM_MSG_T));
    NRF_openWritePipe(TXAddr);
    count++;

}

void comm_deinit_NRF()
{
    count--;
    if(count)
    {
        return;
    }
    NRF_closeReadPipe(1);
    NRF_closeWritePipe();
    NRF_moduleDisable();
}

int32_t comm_sendNRF_raw(uint8_t *data, uint32_t len)
{
    if(len <= 32)
    {
        NRF_transmit_data(data, len, true);
    }
//    else
//    {
//        size_t i = 0;
//        while(i < len)
//        {
//            NRF_transmit_data(data+i, 32 - (i%32), false);
//            i = i+32;
//        }
//    }

}

int32_t comm_recvNRF_raw(uint8_t *data, size_t len)
{

}
int32_t comm_recvNRF(COMM_MSG_T *p_comm_object)
{
    return NRF_read_data((uint8_t*)p_comm_object, sizeof(COMM_MSG_T));
}

#endif
