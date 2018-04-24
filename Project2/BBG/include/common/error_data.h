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

#define ERROR   "[ERROR] "
#define INFO    "[INFO] "
#define SIGNAL  "[SIGNAL] "
#define WARNING "[WARNING] "


#endif