/**
 * @brief 
 * 
 * @file error_data.h
 * @author Gunj Manseta
 * @date 2018-03-09
 */

#ifndef ERROR_DATA_H
#define ERROR_DATA_H

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>


typedef enum{

    ERR     = -1,
    SUCCESS = 0,
}RETURN_T;

//syscall(SYS_gettid) [TID:%ld] ",getpid()

//#define LOG_STDOUT(format, ...) printf("[PID:%d]",getpid()); printf(format, ##__VA_ARGS__)
#define LOG_STDOUT(format, ...)     do{printf("[PID:%d][TID:%ld]",getpid(),syscall(SYS_gettid)); printf(format, ##__VA_ARGS__); fflush(stdout);}while(0)

#define LOG_STDOUT_COMM(recv_comm_msg)   \
            ({LOG_STDOUT(INFO "\n*******\
            \nSRCID:%u, SRC_BRDID:%u, DST_ID:%u, DST_BRDID:%u MSGID:%u\
            \nSensorVal: %.2f MSG:%s\
            \nChecksum:%u ?= %u\n********\n",\
            recv_comm_msg.src_id, recv_comm_msg.src_brd_id, recv_comm_msg.dst_id,recv_comm_msg.dst_brd_id,recv_comm_msg.msg_id,recv_comm_msg.data.distance_cm,recv_comm_msg.message,recv_comm_msg.checksum, check );})   

#define ERROR   "[ERROR] "
#define INFO    "[INFO] "
#define SIGNAL  "[SIGNAL] "
#define WARNING "[WARNING] "


#endif