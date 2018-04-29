/**
 * @brief 
 * 
 * @file sensor_common_object.h
 * @author Gunj Manseta
 * @date 2018-03-11
 */
#ifndef SENS_COMM_OBJ_H
#define SENS_COMM_OBJ_H

#include <stdint.h>
#include <semaphore.h>


/***
 * Required for remote client server communication 
 **/
typedef enum
{
    GET_TEMP_C,
    GET_TEMP_F,
    GET_TEMP_K,
    GET_LUX,
    GET_DAY_NIGHT,
    GET_FUNC,
    GET_DISTANCE_CM,
    GET_DISTANCE_M,
    CONN_CLOSE_REQ,
    CONN_CLOSE_RSP,
    CONN_KILL_APP_REQ,
    CONN_KILL_APP_RSP

}REMOTE_REQRSP_ID;

typedef struct
{
    REMOTE_REQRSP_ID request_id;

}REMOTE_REQUEST_T;

typedef struct
{
    REMOTE_REQRSP_ID rsp_id;
    union data{
        float floatingData;
        uint8_t isNight;
    }data;
    char metadata[20];

}REMOTE_RESPONSE_T;



typedef uint8_t*    P_BUFF_T;
typedef uint8_t     DEV_REG_T;
typedef size_t      BUFF_LEN_T;

typedef enum   
{
    ASYNC = 0,
    SYNC  = 1
}SYNC_TYPE_T;

typedef struct
{
    SYNC_TYPE_T is_sync;   
    sem_t       *sync_semaphore;
    DEV_REG_T   dev_addr;
    P_BUFF_T    *reg_value;
    BUFF_LEN_T  buffLen;

}OBJECT_PACKET_T;

#define SENSOR_MAKE_PACKET_SYNCTYPE(p_packet, p_sem) { p_packet->is_sync = SYNC; if(NULL != p_sem){p_packet->sync_semaphore = p_sem;} }

#define SENSOR_MAKE_PACKET_ASYNCTYPE(p_packet)   {p_packet->is_sync = ASYNC; p_packet->sync_semaphore = NULL;}

#define SENSOR_MAKE_PACKET_RW_1DATA(p_packet)   (p_packet->buffLen = 1)
#define SENSOR_MAKE_PACKET_RW_2DATA(p_packet)   (p_packet->buffLen = 2)

static inline int  SENSOR_FILL_OBJECT_DATA(OBJECT_PACKET_T *packet, DEV_REG_T devaddr, P_BUFF_T *val, BUFF_LEN_T len)
{
    packet->dev_addr = devaddr;
    packet->reg_value = val;
    packet->buffLen = len;
}


#endif