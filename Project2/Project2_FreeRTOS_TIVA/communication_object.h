/*
 * communication_object.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef COMMUNICATION_OBJECT_H_
#define COMMUNICATION_OBJECT_H_


#define GET_BOARD_UID_FROM_LOG_ID(id)   ((uint32_t)((id & (0xFFU<<24))>>24))
#define GET_LOG_ID_FROM_LOG_ID(id)      ((id & (~(0xFFU<<24))))

typedef enum
{
    HEARTBEAT = 0,
    MSG,
    STATUS,
    ERROR,
    INFO,
    CLIENT_INFO_BOARD_TYPE,
    CLIENT_INFO_UID,
    CLIENT_INFO_CODE_VERSION,

}COMM_ID;


/* 32byte LOG MESSAGE STRUCTURE*/
typedef struct COMM_MSG
{
    COMM_ID ID;
    char payload[26];
    uint16_t checksum;

}COMM_MSG_T;



static uint16_t getCheckSum(const COMM_MSG_T *comm_msg)
{
    uint16_t checkSum = 0;
    uint8_t sizeOfPayload = strlen(comm_msg->payload) + sizeof(comm_msg->ID);
    uint8_t *p_payload = (uint8_t*)comm_msg;
    int i;
    for(i = 0; i < sizeOfPayload; i++)
    {
        checkSum += *(p_payload+i);
    }
    return checkSum;
}


#endif /* COMMUNICATION_OBJECT_H_ */
