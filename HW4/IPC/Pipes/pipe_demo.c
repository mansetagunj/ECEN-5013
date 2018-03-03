#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
typedef struct{
    char buffer[20];
    size_t bufferLen;
    uint8_t usrLed_onoff:1;
}payload_t;

#define LOG(format, ...) printf("[PID:%d] ",getpid()); printf(format, ##__VA_ARGS__)

int main()
{
    // We use two pipes
    // First pipe to send input string from parent
    // Second pipe to send concatenated string from child
 
    int Par_to_Ch[2];  // Used to store two ends of first pipe
    int Ch_to_Par[2];  // Used to store two ends of second pipe
 
    pid_t p;
 
    if (pipe(Par_to_Ch)==-1)
    {
        LOG("[ERROR] Pipe call error\n");
        return 1;
    }
    if (pipe(Ch_to_Par)==-1)
    {
        LOG("[ERROR] Pipe call error\n");
        return 1;
    }
 
    p = fork();
 
    if (p < 0)
    {
        LOG("[ERROR] Fork error\n");
        return 1;
    }
 
    // Parent process
    else if (p > 0)
    {
        LOG("PARENT\n");
        char concat_str[100];
        const char* msg = "Hello from Parent";
        char *payload_cptr;
        payload_t payloadSend = {0};
        payload_cptr = (char*)&payloadSend;

        memcpy(payloadSend.buffer,msg,strlen(msg)+1);
        payloadSend.bufferLen = strlen(payloadSend.buffer);
        payloadSend.usrLed_onoff = 1;
 
        close(Par_to_Ch[0]);  
 
        write(Par_to_Ch[1], payload_cptr, sizeof(payloadSend));
        LOG("[INFO] Message sent to child from parent\n");
        close(Par_to_Ch[1]);
 
        /* Wait for child to send a string */
        wait(NULL);
 
        close(Ch_to_Par[1]); 
 
        char readbuf[sizeof(payload_t)] = {0};
        payload_t *payloadptr;

        read(Ch_to_Par[0], readbuf, sizeof(payload_t));

        payloadptr = (payload_t*)readbuf;
        LOG("[INFO] Message Recvd\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadptr->buffer,payloadptr->bufferLen,payloadptr->usrLed_onoff);

        close(Ch_to_Par[0]);
    }
    /* child process */
    else
    {
        LOG("CHILD\n");
        close(Par_to_Ch[1]);
 
        char readbuf[sizeof(payload_t)] = {0};
        payload_t *payloadptr;

        read(Par_to_Ch[0], readbuf, sizeof(payload_t));
 
        payloadptr = (payload_t*)readbuf;
        LOG("[INFO] Message Recvd\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",payloadptr->buffer,payloadptr->bufferLen,payloadptr->usrLed_onoff);
 
        close(Par_to_Ch[0]);
 
        close(Ch_to_Par[0]);

        const char* msg = "Hello from Child";
        char *payload_cptr;
        payload_t payloadSend = {0};
        payload_cptr = (char*)&payloadSend;

        memcpy(payloadSend.buffer,msg,strlen(msg)+1);
        payloadSend.bufferLen = strlen(payloadSend.buffer);
        payloadSend.usrLed_onoff = 0;
 
        write(Ch_to_Par[1], payload_cptr, sizeof(payloadSend));
        LOG("[INFO] Message sent to parent from child\n");
 
        close(Ch_to_Par[1]);
 
        exit(0);
    }
}