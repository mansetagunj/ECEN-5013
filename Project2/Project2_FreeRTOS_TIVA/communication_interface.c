/*
 * communication_interface.c
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */


#include "communication_interface.h"

/* NRF COMM FUNCTIONS*/
void my_NRF_IntHandler()
{
}

void comm_init_NRF()
{
    NRF_moduleInit(NRF_USE_INTERRUPT, my_NRF_IntHandler);
    NRF_moduleSetup(NRF_DR_1Mbps, NRF_PW_MED);
    NRF_openReadPipe(1, RXAddr, sizeof(COMM_MSG_T)>32 ? 32 : sizeof(COMM_MSG_T));
    NRF_openWritePipe(TXAddr);
}

void comm_sendNRF_raw(uint8_t *data, uint32_t len)
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
