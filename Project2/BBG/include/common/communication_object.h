/*
 * communication_object.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef COMMUNICATION_OBJECT_H_
#define COMMUNICATION_OBJECT_H_

#include <string.h>

#ifndef BOARD_UID_SHIFT
#define BOARD_UID_SHIFT 24
#endif

#define GET_BOARD_UID_FROM_LOG_ID(id)   ((uint32_t)((id & (0xFFU<<BOARD_UID_SHIFT))>>BOARD_UID_SHIFT))
#define GET_LOG_ID_FROM_LOG_ID(id)      ((id & (~(0xFFU<<BOARD_UID_SHIFT))))


typedef enum
{
    MSG_ID_HEARTBEAT = 0,
    MSG_ID_MSG,
    MSG_ID_SENSOR_STATUS,
    MSG_ID_ERROR,
    MSG_ID_SENSOR_INFO,
    MSG_ID_INFO,
    MSG_ID_PICTURE,
    MSG_ID_OBJECT_DETECTED,
    MSG_ID_CLIENT_INFO_BOARD_TYPE,
    MSG_ID_CLIENT_INFO_UID,
    MSG_ID_CLIENT_INFO_CODE_VERSION,

    //The request id from the beaglebone
    MSG_ID_GET_SENSOR_STATUS,
    MSG_ID_GET_SENSOR_INFO,
    MSG_ID_GET_CLIENT_INFO_BOARD_TYPE,
    MSG_ID_GET_CLIENT_INFO_UID,
    MSG_ID_GET_CLIENT_INFO_CODE_VERSION,
    LAST_ID, //THIS ID IS JUST TO CALCULATE THE NUM OF IDS. THIS IS NOT USED ANYWHERE This cannot be more than 255
}MSG_ID_T;

#define NUM_OF_ID   LAST_ID

const static char * const MSG_ID_STRING[NUM_OF_ID]  =
{
    "HEARTBEAT",
    "MSG",
    "STATUS",
    "ERROR",
    "INFO",
    "PICTURE",
    "OBJECT_DETECTED",
    "CLIENT_INFO_BOARD_TYPE",
    "CLIENT_INFO_UID",
    "CLIENT_INFO_CODE_VERSION",
    //The request id from the beaglebone
    "GET_SENSOR_STATUS",
    "GET_SENSOR_INFO",
    "GET_CLIENT_INFO_BOARD_TYPE",
    "GET_CLIENT_INFO_UID",
    "GET_CLIENT_INFO_CODE_VERSION",
};

//FOR DST and SRC Board ID
#define BBG_BOARD_ID        (0x00)
#define TIVA_BOARD1_ID      (0x01)
#define XYZ_TIVA_BOARD_ID   (0x02)

#define MY_TIVA_BOARD_ID    TIVA_BOARD1_ID

//For src and dst module ID
//Add all the modules' UID here for TIVA BOARD
#define TIVA_HEART_BEAT_MODULE   (1)
#define TIVA_SENSOR_MODULE       (2)
#define TIVA_CAMERA_MODULE       (3)
#define TIVA_COMM_MODULE         (4)

//Add all modules' UID here for BBG Board
#define BBG_LOGGER_MODULE       (1)
#define BBG_COMM_MODULE         (2)
#define BBG_SOCKET_MODULE       (3)
#define BBG_XYZ_MODULE          (4)

typedef uint8_t MSG_ID;
typedef uint8_t SRC_ID;
typedef uint8_t SRC_BOARD_ID;
typedef uint8_t DST_BOARD_ID;
typedef uint8_t DST_ID;

//This should be followed immediately by the PICTURE msg id
typedef struct cam_packet
{
    size_t length;
    void* frame;
}CAMERA_PACKET_T;

/*32byte LOG MESSAGE STRUCTURE*/
typedef struct COMM_MSG
{
    SRC_ID src_id;
    SRC_BOARD_ID src_brd_id;
    DST_ID dst_id;
    DST_BOARD_ID dst_brd_id;
    MSG_ID msg_id;
    union custom_data
    {
        float distance_cm;
        float sensor_value;
        CAMERA_PACKET_T *camera_packet;
        size_t nothing;
    }data;
    char message[18];
    uint16_t checksum;
}COMM_MSG_T;

static size_t COMM_MSG_SIZE = sizeof(COMM_MSG_T);

static uint16_t getCheckSum(const COMM_MSG_T *comm_msg)
{
    uint16_t checkSum = 0;
    uint8_t sizeOfPayload = sizeof(COMM_MSG_T) - sizeof(comm_msg->checksum);
    uint8_t *p_payload = (uint8_t*)comm_msg;
    int i;
    for(i = 0; i < sizeOfPayload; i++)
    {
        checkSum += *(p_payload+i);
    }
    return checkSum;
}

/*Return true if a match, return 0 is not a match*/
static inline uint8_t verifyCheckSum(const COMM_MSG_T *comm_msg)
{
    return (getCheckSum(comm_msg) == comm_msg->checksum);
}


#endif /* COMMUNICATION_OBJECT_H_ */
