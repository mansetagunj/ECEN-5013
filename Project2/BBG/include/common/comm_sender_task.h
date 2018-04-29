/**
 * @brief 
 * 
 * @file comm_sender_task.h
 * @author Gunj Manseta
 * @date 2018-04-26
 */

#ifndef COMM_SENDER_H
#define COMM_SENDER_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>

#include "common_helper.h"
#include "communication_object.h"

#define COMM_CREATE_OBJECT(name, src_board_id, source_id, dest_id)    COMM_MSG_T name = { .src_brd_id = src_board_id, .src_id = source_id, .dst_id = dest_id, .dst_brd_id = TIVA_BOARD1_ID }
#define COMM_OBJECT_MSGID(comm_msg,msgid)   comm_msg.msg_id = msgid
#define COMM_DST_BOARD_ID(comm_msg,dst_board_id)    comm_msg.dst_brd_id = dst_board_id
#define FILL_CHECKSUM(p_comm_msg)           do{ (p_comm_msg)->checksum = getCheckSum(p_comm_msg); }while(0)
#define COMM_FILL_MSG(comm_msg,p_str)       strncpy(comm_msg.message,p_str,sizeof(comm_msg.message))

/**
 * @brief Get the Handle CommSenderTaskQueue object
 * 
 * @return mqd_t 
 */
mqd_t getHandle_CommSenderTaskQueue();


#define POST_MESSAGE_COMM_SENDTASK(p_comm_msg, format, ...)  \
    do{ \
        (strlen(format)>0) ? snprintf((p_comm_msg)->message,sizeof((p_comm_msg)->message),format, ##__VA_ARGS__): 0;   \
        FILL_CHECKSUM(p_comm_msg);   \
        __POST_MESSAGE_COMM_SENDTASK(getHandle_CommSenderTaskQueue(), (p_comm_msg), sizeof(*p_comm_msg), 20); \
    }while(0)

#define POST_MESSAGE_COMM_SENDTASK_EXIT(format, ...)  \
    do{ \
        COMM_MSG_T comm_msg;    \
        (strlen(format)>0) ? snprintf(comm_msg.message,sizeof(comm_msg.message),format, ##__VA_ARGS__):0; \
        COMM_OBJECT_MSGID(comm_msg,0xFF); \
        COMM_DST_BOARD_ID(comm_msg,BBG_BOARD_ID); \
        __POST_MESSAGE_COMM_SENDTASK(getHandle_CommSenderTaskQueue(), &comm_msg, sizeof(comm_msg), 20); \
    }while(0)

/**
 * @brief 
 * 
 * @param queue 
 * @param comm_msg 
 * @param comm_msg_size 
 * @param prio 
 */
static inline void __POST_MESSAGE_COMM_SENDTASK(mqd_t queue, const COMM_MSG_T *comm_msg, size_t comm_msg_size, int prio)
{
    if(-1 == mq_send(queue, (const char*)comm_msg, comm_msg_size, prio))
    {
        LOG_STDOUT(ERROR "COMM_SEND:MQ_SEND:%s\n",strerror(errno));
    }
}

/**
 * @brief 
 * 
 * @param board_id 
 */
static inline void send_GET_CLIENT_INFO_CODE_VERSION(uint8_t board_id)
{
    COMM_CREATE_OBJECT(comm_msg,BBG_BOARD_ID, BBG_XYZ_MODULE, TIVA_COMM_MODULE);
    COMM_OBJECT_MSGID(comm_msg,MSG_ID_CLIENT_INFO_CODE_VERSION);
    COMM_DST_BOARD_ID(comm_msg,board_id);
    FILL_CHECKSUM(&comm_msg); 
    POST_MESSAGE_COMM_SENDTASK((&comm_msg), "BBG/Req/CodeV");
}

/**
 * @brief 
 * 
 * @param board_id 
 */
static inline void send_GET_CLIENT_INFO_BOARD_TYPE(uint8_t board_id)
{
    COMM_CREATE_OBJECT(comm_msg,BBG_BOARD_ID, BBG_XYZ_MODULE, TIVA_COMM_MODULE);
    COMM_OBJECT_MSGID(comm_msg,MSG_ID_CLIENT_INFO_BOARD_TYPE);
    COMM_DST_BOARD_ID(comm_msg,board_id);
    FILL_CHECKSUM(&comm_msg);
    POST_MESSAGE_COMM_SENDTASK(&comm_msg, "BBG/Req/BType");
}

/**
 * @brief 
 * 
 * @param board_id 
 */
static inline void send_GET_CLIENT_INFO_UID(uint8_t board_id)
{
    COMM_CREATE_OBJECT(comm_msg,BBG_BOARD_ID, BBG_XYZ_MODULE, TIVA_COMM_MODULE);
    COMM_OBJECT_MSGID(comm_msg,MSG_ID_GET_CLIENT_INFO_UID);
    COMM_DST_BOARD_ID(comm_msg,board_id);
    FILL_CHECKSUM(&comm_msg);
    POST_MESSAGE_COMM_SENDTASK(&comm_msg, "BBG/Req/UID");
}

/**
 * @brief 
 * 
 * @param board_id 
 */
static inline void send_GET_DISTANCE(uint8_t board_id, uint8_t src_module_id)
{
    COMM_CREATE_OBJECT(comm_msg,BBG_BOARD_ID, src_module_id, TIVA_SENSOR_MODULE);
    COMM_OBJECT_MSGID(comm_msg,MSG_ID_GET_SENSOR_STATUS);
    COMM_DST_BOARD_ID(comm_msg,board_id);
    FILL_CHECKSUM(&comm_msg);
    POST_MESSAGE_COMM_SENDTASK(&comm_msg, "BBG/Req/Distance");
}


/**
 * @brief 
 * 
 * @param threadparam 
 * @return void* 
 */
void* comm_sender_task_callback(void *threadparam);

#endif
