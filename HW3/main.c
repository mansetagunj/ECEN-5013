#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>

#include "letterParser.h"
#include "time.h"

#define LOG_FILENAME 				"Homework3.log"
#define LOG_INIT(filename)			FILE *fp = fopen(filename,"a+")
#define GET_LOG_HANDLE() 			(fp)
#define LOG(format,...) 			do { /*LOG_INIT(LOG_FILENAME);*/ LOG_THREAD_IDENTIFIER(); fprintf(GET_LOG_HANDLE(),format, ##__VA_ARGS__);fflush(GET_LOG_HANDLE()); } while(0)
#define LOG_CLOSE() 				fclose(GET_LOG_HANDLE())

#define PRINT_THREAD_IDENTIFIER()	printf("[PID:%ld] [TTID:%ld] [Pthread_TID:%lu] ",(long int)getpid(), (long int)syscall(SYS_gettid), pthread_self())
#define LOG_THREAD_IDENTIFIER() 	fprintf(GET_LOG_HANDLE(),"[PID:%ld] [TTID:%ld] [Pthread_TID:%lu] ",(long int)getpid(), (long int)syscall(SYS_gettid), pthread_self())

struct threadParams{

    pthread_t threadId;
    char *info;
    char *filename;
};

sem_t gotSignal_sem;



void* callBack_thread0(void* params)
{
	struct threadParams *inParams = (struct threadParams*)params;
	pthread_t self_pthreadId = pthread_self();
	pid_t process_id = getpid();
	pid_t linux_threadID = syscall(SYS_gettid);

	LOG_INIT(inParams->filename);
	if(!GET_LOG_HANDLE())
		printf("File open error\n");

	char timeString[40] = {0};
	if(get_time_string(timeString) == 0)
		LOG("[ENTRY TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	LOG("Setup of Thread0 done\n");


	/* TO DO - Add functions for parsing Valentines.txt */
	parser_parse(

	LOG("Waiting for SIGUSR.\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Waiting for SIGUSR.\n");

	sem_wait(&gotSignal_sem);

	
	LOG("Exiting Thread 0\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Exiting thread 0.\n");

	if(get_time_string(timeString) == 0)
		LOG("[EXIT TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();

	sem_post(&gotSignal_sem);
}

#if 0 

struct thread_cleanup{

	FILE *fp;
	void *heapMemArray;

};

void thread1_cleanup(void *arg)
{
	/* We need to clear the dynamic memory and file pointers */
	struct thread_cleanup *cleanup_mem = (struct thread_cleanup*)arg;

	if(cleanup_mem->fp)
	{
		fclose(cleanup_mem->fp);
		cleanup_mem->fp = NULL;
	}

	LOG("Exiting Thread 1 from Cleanup\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Exiting thread 1 from Cleanup.\n");

	/* TO DO -Free any heap memory */

}

#endif


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
		LOG("[CPU UTILIZATION %%] %s\n",cpu_utilization_buff);	\
		pclose(pipe_p);	\
	}	\
	else	\
		LOG("[ERROR] Cannot open Pipe.\n");	\
	}while(0)

void* callBack_thread1(void* params)
{
	struct threadParams *inParams = (struct threadParams*)params;
	pthread_t self_pthreadId = pthread_self();
	pid_t process_id = getpid();
	pid_t linux_threadID = syscall(SYS_gettid);
	pthread_t self = pthread_self();

	LOG_INIT(inParams->filename);
	if(!GET_LOG_HANDLE())
		printf("File open error\n");

	char timeString[40] = {0};
	if(get_time_string(timeString) == 0)
		LOG("[ENTRY TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	
	LOG("Setup of Thread1 done\n");

	LOG("Registering cleanup function\n");

	//struct thread_cleanup cleanup_struct = { .fp = GET_LOG_HANDLE() , .heapMemArray = NULL  };
	//pthread_cleanup_push(thread1_cleanup,(void*)&cleanup_struct);

	/* TO DO - Create and start 100ms timer with callback which prints CPU utilization */

	LOG("Waiting for SIGUSR.\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Waiting for SIGUSR.\n");

	while(1)
	{
		if(sem_trywait(&gotSignal_sem) == 0)
		{
			printf("Got semaphore from try wait.\n");
			break;
		}
		LOG_CPU_UTILIZATION();
		sleep(5);
		//nanosleep(100000);
	}

	/* Waiting for SIGUSR1 or SIGUSR2. Which releases the semaphore */
	//sem_wait(&gotSignal_sem);

	
	LOG("Exiting Thread 1\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Exiting thread 1.\n");

	if(get_time_string(timeString) == 0)
		LOG("[EXIT TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();
	
	/* Release the semaphore to be used by other thread */
	sem_post(&gotSignal_sem);
}


void signal_handler(int signal)
{
	switch (signal)
	{

		case SIGUSR1:
			printf("SIGUSR1 signal.\n");
			//LOG("SIGUSR1 signal\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGUSR2:
			printf("SIGUSR2 signal.\n");
			//LOG("SIGUSR2 signal\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGINT:
			printf("SIGINT signal.\n");
			//LOG("SIGINT signal\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGTERM:
			printf("SIGTERM signal.\n");
			//LOG("SIGTERM signal\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGTSTP:
			printf("SIGTSTP signal.\n");
			//LOG("SIGTSTP signal\n");
			sem_post(&gotSignal_sem);
			break;
		default:
			printf("Invalid signal.\n");
			//LOG("Invalid signal\n");
			break;
	}
}

int register_signalHandler(struct sigaction *sa,void (*handler)(int))
{
	sa->sa_handler = handler;

	sa->sa_flags = SA_RESTART;

	sigfillset(&sa->sa_mask);

	int ret_error = 0;
	
	if (sigaction(SIGUSR1, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGUSR1.\n");
		//LOG("Cannot handle SIGUSR1.\n");
	}

	if (sigaction(SIGUSR2, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGUSR2.\n");
		//LOG("Cannot handle SIGUSR2.\n");
	}
	
	if (sigaction(SIGINT, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGINT.\n");
		//LOG("Cannot handle SIGUSR1.\n");
	}
	
	if (sigaction(SIGTERM, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGTERM.\n");
		//LOG("Cannot handle SIGUSR1.\n");
	}
	
	if (sigaction(SIGTSTP, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGTSTOP.\n");
		//LOG("Cannot handle SIGUSR1.\n");
	}

	return ret_error;
}

int main()
{
    pthread_t p_threads[2];
	struct sigaction sa;
    int ret;
    struct threadParams thread_info[2];

	sem_init(&gotSignal_sem,0,0);
    
	LOG_INIT(LOG_FILENAME);
	if(!GET_LOG_HANDLE())
	{
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot open log\n");
		return 1;
	}

	LOG("Log initialized.\n");
    
	register_signalHandler(&sa,signal_handler);

	thread_info[0].threadId 	= 0;
    thread_info[0].info 	 	= "Thread0";
	thread_info[0].filename 	= LOG_FILENAME;

    thread_info[1].threadId		= 1;
    thread_info[1].info	 		= "Thread1";    
	thread_info[1].filename 	= LOG_FILENAME;

	LOG("Creating children Threads.\n");	

	ret = pthread_create(&p_threads[0], NULL, callBack_thread0, (void*)&thread_info[0]);
	if(ret != 0)
	{
		LOG("Cannot create child thread 0\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}

	ret = pthread_create(&p_threads[1], NULL, callBack_thread1, (void*)&thread_info[1]);
	if(ret != 0)
	{
		LOG("Cannot create child thread 1\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}

    LOG("Thread created successfully\n");
	
	/* Waiting on child threads to complete */	
	ret = pthread_join(p_threads[0],NULL);
	if(0 != ret)
	{
		LOG("Pthread JOIN error\n"); printf("Join Error Thread 0\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}
	
	ret	= pthread_join(p_threads[1],NULL);
	if(0 != ret)
	{
		LOG("Pthread JOIN error\n"); printf("Join Error Thread 0\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}

    
	sem_destroy(&gotSignal_sem);
	printf("GoodBye!!\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();

    return EXIT_SUCCESS;
}
