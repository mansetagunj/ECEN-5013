#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>


#include "letterParser.h"
#include "my_time.h"
#include "my_signals.h"
#include "threadManager.h"
#include "posixTimer.h"

#include "log_macros.h"


#define LOG_FILENAME 	"Homework3.log"
#define TEXT_FILENAME	"Valentinesday.txt"

sem_t gotSignal_sem;
sem_t gotTimerSignal_sem;

struct threadParams{

    pthread_t threadId;
    char *info;
    char *filename;
};

static void signal_handler(int signal)
{
	switch (signal)
	{

		case SIGUSR1:
			STDOUT_LOG("\n[SIGNAL] SIGUSR1 signal.\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGUSR2:
			STDOUT_LOG("\n[SIGNAL] SIGUSR2 signal.\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGINT:
			STDOUT_LOG("\n[SIGNAL] SIGINT signal.\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGTERM:
			STDOUT_LOG("\n[SIGNAL] SIGTERM signal.\n");
			sem_post(&gotSignal_sem);
			break;
		case SIGTSTP:
			STDOUT_LOG("\n[SIGNAL] SIGTSTP signal.\n");
			sem_post(&gotSignal_sem);
			break;
		default:
			STDOUT_LOG("\n[SIGNAL] Invalid signal.\n");
			break;
	}
}

void* callBack_thread0(void* params)
{
	struct threadParams *inParams = (struct threadParams*)params;
	pthread_t self_pthreadId = pthread_self();
	pid_t process_id = getpid();
	pid_t linux_threadID = syscall(SYS_gettid);

	LOG_INIT(inParams->filename);
	if(GET_LOG_HANDLE() == NULL)
	{
		STDOUT_LOG("[ERROR] File open error\n");
	}

	char timeString[40] = {0};
	if(get_time_string(timeString) == 0)
		LOG("[ENTRY TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	LOG("[INFO] Setup of Thread0 done\n");


	/* TO DO - Add functions for parsing Valentines.txt */
	PARSER_LETTER_T *letter_list = NULL;
	int ret = parser_parse(TEXT_FILENAME,&letter_list);
	if(ret == 0)
	{
		letterType *inout_elemArray = NULL;
		size_t numofElems = get_occurenceN_letters(letter_list, &inout_elemArray, 3);
	
		LOG("[INFO] Found %u chars with 3 occurence. -",numofElems);
		STDOUT_LOG("Found %u chars with 3 occurence. -",numofElems);
	
		for(int  i = 0; i < numofElems && (inout_elemArray+i); i++)
		{
			LOG_PLAIN("[%c]",inout_elemArray[i]);
			STDOUT_LOG_PLAIN("[%c]",inout_elemArray[i]);
		}
		STDOUT_LOG_PLAIN("\n");
		LOG_PLAIN("\n");
		
		cleanup_parser(letter_list);
	}
	else
	{
		LOG("[ERROR} PARSING\n");
		STDOUT_LOG("[ERROR] PARSING\n");
	
	}
	
	LOG("[INFO] Waiting for SIGUSR.\n");
	STDOUT_LOG("[INFO] Waiting for SIGUSR.\n");

	sem_wait(&gotSignal_sem);

	
	LOG("[INFO] Exiting Thread 0\n");
	STDOUT_LOG("[INFO] Exiting thread 0.\n");

	if(get_time_string(timeString) == 0)
		LOG("[EXIT TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();

	sem_post(&gotSignal_sem);
}

static void timer_handler(union sigval sig)
{		
	sem_post(&gotTimerSignal_sem);
}

void* callBack_thread1(void* params)
{
	struct threadParams *inParams = (struct threadParams*)params;
	pthread_t self_pthreadId = pthread_self();
	pid_t process_id = getpid();
	pid_t linux_threadID = syscall(SYS_gettid);
	pthread_t self = pthread_self();

	sem_init(&gotTimerSignal_sem,0,0);
	
	LOG_INIT(inParams->filename);
	if(GET_LOG_HANDLE() == NULL)
	{
		STDOUT_LOG("[ERROR] File open error\n");
	}

	char timeString[40] = {0};
	if(get_time_string(timeString) == 0)
		LOG("[ENTRY TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	timer_t timer_id;
	
	if(register_timer(&timer_id, timer_handler,&timer_id) == -1)
	{
		LOG("[ERROR] Register Timer\n");
		//exit (1);
	}
	else
		LOG("[INFO] Timer created\n");
	
	if(start_timer(timer_id , 500, 0) == -1)
	{
		LOG("[ERROR] Start Timer\n");
		//exit (1);
	}
	else
		LOG("[INFO] Timer started\n");
	
	LOG("[INFO] Setup of Thread1 done\n");


	LOG("[INFO] Waiting for SIGUSR.\n");
	STDOUT_LOG("INFO] Waiting for SIGUSR.\n");


	while(1)
	{
		if(sem_trywait(&gotSignal_sem) == 0)
		{
			STDOUT_LOG("[INFO] Got semaphore from try wait.\n");
			break;
		}
		if(sem_trywait(&gotTimerSignal_sem) == 0)
		{
			LOG_CPU_UTILIZATION();
		}
	}


	/* Waiting for SIGUSR1 or SIGUSR2. Which releases the semaphore */
	//sem_wait(&gotSignal_sem);

	if(delete_timer(timer_id) == -1)
	{
		LOG("[ERROR] Delete Timer\n");
		//exit (1);
	}
	else
		LOG("[INFO] Timer deleted\n");
		
	sem_destroy(&gotTimerSignal_sem);

	if(get_time_string(timeString) == 0)
		LOG("[EXIT TIME][THREAD1] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();
		
	STDOUT_LOG("[INFO] Exiting thread 1.\n");
	
	/* Release the semaphore to be used by other thread */
	sem_post(&gotSignal_sem);
}




int threadManager_startThreads()
{
	pthread_t p_threads[2];
	struct sigaction sa;
    int ret;
    struct threadParams thread_info[2];

	sem_init(&gotSignal_sem,0,0);
    
	LOG_INIT(LOG_FILENAME);
	if(!GET_LOG_HANDLE())
	{
		STDOUT_LOG("[ERROR] Cannot open log\n");
		return 1;
	}

	LOG("[INFO] Log initialized.\n");
    
    /*Registering the signal callback handler*/
	register_signalHandler(&sa,signal_handler, REG_SIG_ALL);

	thread_info[0].threadId 	= 0;
    thread_info[0].info 	 	= "Thread0";
	thread_info[0].filename 	= LOG_FILENAME;

    thread_info[1].threadId		= 1;
    thread_info[1].info	 		= "Thread1";    
	thread_info[1].filename 	= LOG_FILENAME;

	LOG("[INFO] Creating children Threads.\n");	

	ret = pthread_create(&p_threads[0], NULL, callBack_thread0, (void*)&thread_info[0]);
	if(ret != 0)
	{
		LOG("[ERROR] Cannot create child thread 0\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}

	ret = pthread_create(&p_threads[1], NULL, callBack_thread1, (void*)&thread_info[1]);
	if(ret != 0)
	{
		LOG("[ERROR] Cannot create child thread 1\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}

    LOG("[INFO] Thread created successfully\n");
	
	/* Waiting on child threads to complete */	
	ret = pthread_join(p_threads[0],NULL);
	if(0 != ret)
	{
		LOG("[ERROR] Pthread JOIN error\n"); 
		STDOUT_LOG("[ERROR] Join Error Thread 0\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}
	
	ret	= pthread_join(p_threads[1],NULL);
	if(0 != ret)
	{
		LOG("[ERROR] Pthread JOIN error\n"); 
		STDOUT_LOG("[ERROR] Join Error Thread 0\n");
		if(GET_LOG_HANDLE())
			LOG_CLOSE();
		return 1;
	}

    
	sem_destroy(&gotSignal_sem);
	LOG("[INFO] GoodBye!!\n");
	STDOUT_LOG("[INFO] GoodBye!!\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();

    return EXIT_SUCCESS;


}
