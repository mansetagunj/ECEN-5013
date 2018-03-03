#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#define SH_MEM_NAME	"/MY_SH_MEM"
#define SH_MEM_SIZE sizeof(payload_t)

#define SEM_NAME "/sharedMemSemaphore"

#define LOG(format, ...) printf("[PID:%d] ",getpid()); printf(format, ##__VA_ARGS__)

typedef struct{
    char buffer[20];
    size_t bufferLen;
    uint8_t usrLed_onoff:1;
}payload_t;


int main()
{
	LOG("[INFO] Starting the process 1\n");	
	int shmem_fd = shm_open(SH_MEM_NAME, O_CREAT | O_RDWR, 0666);
	if(shmem_fd < 0 )
	{
		LOG("[ERROR] Cannot open Shared Mem: %s",strerror(errno));
		return -1;
	}

	void *shared_mem = mmap(NULL, SH_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , shmem_fd, 0);
	if(shared_mem == (void*)-1)
	{
		LOG("[ERROR] mmap error: %s\n", strerror(errno));
		return -1;
	}

	/* Creating a semaphore to sync the reads and writes between 2 processes */
	sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
	if(SEM_FAILED == sem)
	{
		LOG("[ERROR] Sem open Failed:%s\n",strerror(errno));
		return -1;	
	}

	/* Waiting for the process 1 to post the sem after writing data to the shared mem */
	sem_wait(sem);

	payload_t payloadRecv = {0};
    char *payload_cptr = (char*)&payloadRecv;

    memcpy(payload_cptr,(char*)shared_mem,SH_MEM_SIZE);

    LOG("[INFO] Message From Proc 1 through Shared Mem\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadRecv.buffer,payloadRecv.bufferLen,payloadRecv.usrLed_onoff);

    const char* msg = "Hello from Process2";
    payload_t payloadSend = {0};
    payload_cptr = (char*)&payloadSend;

    memmove(payloadSend.buffer,msg,strlen(msg));
    payloadSend.bufferLen = strlen(payloadSend.buffer);
    payloadSend.usrLed_onoff = 1;

    /* Copy the contents of the payload into the share memory */
    memcpy((char*)shared_mem, payload_cptr, SH_MEM_SIZE);

    /*Indicating the process 1 that the data has been written for Process 1 eyes only */
    sem_post(sem);

	/*Closing the shared memory handle*/
	int ret = close(shmem_fd);
	if(ret < 0)
	{
		LOG("[ERROR] Cannot close Shared Mem: %s",strerror(errno));
		return -1;
	}
	
	return 0;
}