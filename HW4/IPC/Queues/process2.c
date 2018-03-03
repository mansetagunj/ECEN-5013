#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define SH_Q_NAME 		"/myIPCSharedQ"
#define SH_Q_QSIZE		8

#define MUTEX_NAME		"/mySharedRWMutex"
#define CONDVAR_NAME	"/mySharedRWCondVar"

#define LOG(format, ...) printf("[PID:%d] ",getpid()); printf(format, ##__VA_ARGS__)

typedef struct{
    char buffer[20];
    size_t bufferLen;
    uint8_t usrLed_onoff:1;
}payload_t;

int main()
{
	struct mq_attr mysharedQ_attr;
	mysharedQ_attr.mq_maxmsg = SH_Q_QSIZE;
	mysharedQ_attr.mq_msgsize = sizeof(payload_t);

	mqd_t mySharedQ = mq_open(SH_Q_NAME, O_CREAT | O_RDWR, 0666, &mysharedQ_attr);

	if(mySharedQ == (mqd_t)-1)
	{
		LOG("[ERROR] QUEUE OPEN ERROR.: %s\n",strerror(errno));
		return -1;
	}

    char *payload_cptr;
    payload_t payloadRecv = {0};
    payload_cptr = (char*)&payloadRecv;

	int ret = mq_receive(mySharedQ, payload_cptr, sizeof(payloadRecv),0);
	if(ret == -1)
	{
		LOG("[ERROR] Q Send error: %s\n",strerror(errno));
		return -1;
	}
	LOG("[INFO] Message recd size: %d\n",ret);
	//payloadptr = (payload_t*)readbuf;
    LOG("[INFO] Message Dequeued\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadRecv.buffer,payloadRecv.bufferLen,payloadRecv.usrLed_onoff);


    const char* msg = "Hello from Process2";
    payload_t payloadSend = {0};
    payload_cptr = (char*)&payloadSend;

    memmove(payloadSend.buffer,msg,strlen(msg));
    payloadSend.bufferLen = strlen(payloadSend.buffer);
    payloadSend.usrLed_onoff = 1;

	ret = mq_send(mySharedQ, payload_cptr, sizeof(payloadSend),0);
	if(ret == -1)
	{
		LOG("[ERROR] Q Send error: %s\n",strerror(errno));
		return -1;
	}

	LOG("[INFO] Message Queued\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadSend.buffer,payloadSend.bufferLen,payloadSend.usrLed_onoff);


	/*Closing the Q. Process 1 will destroy the queue */
	mq_close(mySharedQ);

	LOG("[ERROR] QUEUE CLOSED\n");
	return 0;
}