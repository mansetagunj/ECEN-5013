#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#define PORT 2000
//#define IP "127.0.0.1"
#define IP "192.168.1.238"
#define LOG(format, ...) printf(format, ##__VA_ARGS__)


typedef struct{

    char buffer[20];
    size_t bufferLen;
    uint8_t usrLed_onoff:1;
}payload_t;

int main()
{
    struct sockaddr_in addr, server_addr = {0};
    int fd = 0;
    const char* msg = "Hello from Client";
    char *payload_ptr;
    payload_t payloadSend;
    payload_ptr = (char*)&payloadSend;

    memmove(payloadSend.buffer,msg,strlen(msg));
    payloadSend.bufferLen = strlen(payloadSend.buffer);
    payloadSend.usrLed_onoff = 1;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG("[ERROR] Socket creation\n");
        return -1;
    }
  
    LOG("[INFO] Socket Created\n");

    //memset(&server_addr, 0, sizeof(server_addr));
  
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
      
    /* We need this to convert the IP ADDR in proper format */
    if(inet_pton(AF_INET, IP, &server_addr.sin_addr)<=0) 
    {
        LOG("[ERROR] Invalid address\n");
        return -1;
    }
  
    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        LOG("[ERROR] Connection Failed \n");
        return -1;
    }

    /*First sending the size of the incoming payload */
    size_t sizeofPayload = sizeof(payloadSend);
    int bytesSent = send(fd,&sizeofPayload,sizeof(size_t), 0);

    /*Sending the actual payload */
    bytesSent = send(fd , (char*)&payloadSend , sizeof(payloadSend), 0 );

    if(bytesSent < sizeof(payloadSend))
    {
        LOG("[ERROR] Cannot send complete data\n");
        return 1;
    }

    LOG("[INFO] Number of bytes send: %d\n",bytesSent);
    LOG("[INFO] Message sent\nMessage: %s\nMessageLen: %d\nUSRLED: %d\n",payloadSend.buffer,payloadSend.bufferLen,payloadSend.usrLed_onoff);
    
    getchar();
    close(fd);
    return 0;
}