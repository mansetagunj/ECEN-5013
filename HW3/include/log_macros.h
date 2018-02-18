#ifndef __LOG_MACROS_H__
#define __LOG_MACROS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>

#define LOG_INIT(filename)			FILE *fp = fopen(filename,"a+")
#define GET_LOG_HANDLE() 			(fp)
#define LOG(format,...) 			do { /*LOG_INIT(LOG_FILENAME);*/ LOG_THREAD_IDENTIFIER(); fprintf(GET_LOG_HANDLE(),format, ##__VA_ARGS__);fflush(GET_LOG_HANDLE()); } while(0)
#define LOG_CLOSE() 				fclose(GET_LOG_HANDLE())

#define PRINT_THREAD_IDENTIFIER()	printf("[PID:%ld] [TTID:%ld] [Pthread_TID:%lu] ",(long int)getpid(), (long int)syscall(SYS_gettid), pthread_self())
#define LOG_THREAD_IDENTIFIER() 	fprintf(GET_LOG_HANDLE(),"[PID:%ld] [TTID:%ld] [Pthread_TID:%lu] ",(long int)getpid(), (long int)syscall(SYS_gettid), pthread_self())
#define STDOUT_LOG(format, ...)		PRINT_THREAD_IDENTIFIER(); printf(format, ##__VA_ARGS__)

#define LOG_CPU_UTILIZATION() \
	do{	\
	char cpu_utilization_buff[300] = {0};	\
	char *command = "cat /proc/stat | head -n 1"; \
	/*char command = "top -n 1 | head -n 1"; */\
	FILE *pipe_p = popen(command,"r");	\
	if(pipe_p)	\
	{	\
		/*fgets(cpu_utilization_buff,100,pipe_p); */ \
		fread(cpu_utilization_buff,sizeof(char),300,pipe_p);	\
		/*printf("[CPU] %s\n",cpu_utilization_buff);*/	\
		LOG("[CPU UTILIZATION %%] %s",cpu_utilization_buff);	\
		pclose(pipe_p);	\
	}	\
	else	\
		LOG("[ERROR] Cannot open Pipe.\n");	\
	}while(0)
	

#endif
