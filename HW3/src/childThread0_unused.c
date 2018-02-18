#include <pthread.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#include "letterParser.h"
#include "time.h"
#include "log_macros.h"

#define TEXT_FILENAME	"Valentinesday.txt"

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
