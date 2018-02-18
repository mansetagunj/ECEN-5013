





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

	//LOG("Registering cleanup function\n");

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
			PRINT_THREAD_IDENTIFIER();
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
