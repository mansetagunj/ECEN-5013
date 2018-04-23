/*
 * logger_task.h
 *
 *  Created on: 22-Apr-2018
 *      Author: Gunj Manseta
 */

#ifndef LOGGER_TASK_H_
#define LOGGER_TASK_H_

typedef enum
{
    HEARTBEAT = 0,
    MSG,
    STATUS,
    CLIENT_INFO_BOARD_TYPE,
    CLIENT_INFO_UID,
    CLIENT_INFO_CODE_VERSION,
}LOG_ID;

/* 32byte LOG MESSAGE STRUCTURE*/
typedef struct LOG_MSG
{
    LOG_ID ID;
    uint8_t payload[26];
    uint16_t checksum;

}LOG_MSG_T;

#endif /* LOGGER_TASK_H_ */
