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
	PARSER_LETTER_T *letter_list = NULL;
	int ret = parser_parse(TEXT_FILENAME,&letter_list);
	if(ret == 0)
	{
		letterType *inout_elemArray = NULL;
		size_t numofElems = get_occurenceN_letters(letter_list, &inout_elemArray, 3);
	
		LOG("Found %u chars with 3 occurence.\n",numofElems);
		PRINT_THREAD_IDENTIFIER();
		printf("Found %u chars with 3 occurence.\n",numofElems);
	
		for(int  i = 0; i < numofElems && (inout_elemArray+i); i++)
		{
			LOG("Char: %c\n",inout_elemArray[i]);
			printf("[%c]",inout_elemArray[i]);
		}
		
		cleanup_parser(letter_list);
	}
	else
	{
		LOG("[ERROR} PARSING\n");
		PRINT_THREAD_IDENTIFIER();
		printf("[ERROR] PARSING\n");
	
	}
	
	LOG("Waiting for SIGUSR.\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Waiting for SIGUSR.\n");

	//sem_wait(&gotSignal_sem);

	
	LOG("Exiting Thread 0\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Exiting thread 0.\n");

	if(get_time_string(timeString) == 0)
		LOG("[EXIT TIME] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();

	//sem_post(&gotSignal_sem);
}

void timer_handler(union sigval sig)
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
	if(!GET_LOG_HANDLE())
		printf("File open error\n");

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
		LOG("Timer created\n");
	
	if(start_timer(timer_id , 5000000, 0) == -1)
	{
		LOG("[ERROR] Start Timer\n");
		//exit (1);
	}
	else
		LOG("Timer started\n");
	
	LOG("Setup of Thread1 done\n");

	//LOG("Registering cleanup function\n");

	//struct thread_cleanup cleanup_struct = { .fp = GET_LOG_HANDLE() , .heapMemArray = NULL  };
	//pthread_cleanup_push(thread1_cleanup,(void*)&cleanup_struct);


	LOG("Waiting for SIGUSR.\n");
	PRINT_THREAD_IDENTIFIER();
	printf("Waiting for SIGUSR.\n");


	while(1)
	{
		if(sem_trywait(&gotSignal_sem) == 0)
		{
			PRINT_THREAD_IDENTIFIER();
			printf("Got semaphore from try wait.\n");
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
		LOG("Timer deleted\n");
		
	sem_destroy(&gotTimerSignal_sem);

	if(get_time_string(timeString) == 0)
		LOG("[EXIT TIME][THREAD1] %s\n",timeString);
	else
		LOG("[ERROR] Gettimeofday().\n");
	
	if(GET_LOG_HANDLE())
		LOG_CLOSE();
		
	PRINT_THREAD_IDENTIFIER();
	printf("Exiting thread 1.\n");
	
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
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot open log\n");
		return 1;
	}

	LOG("Log initialized.\n");
    
    /*Registering the signal callback handler*/
	register_signalHandler(&sa,signal_handler, REG_SIG_ALL);

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
