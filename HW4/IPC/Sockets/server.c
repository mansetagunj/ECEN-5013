#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 	2000
#define IP		

#define LOG(format, ...) printf(format, ##__VA_ARGS__)

typedef struct{
    char buffer[20];
    size_t bufferLen;
    uint8_t usrLed_onoff:1;
}payload_t;


int main()
{
	int fd, option = 1;
	struct sockaddr_in addr, peer_addr;
	int addrLen = sizeof(peer_addr);
	payload_t payload_recvd;
	if((fd = socket(AF_INET,SOCK_STREAM,0)) == 0)
	{
		LOG("[ERROR] Socket Creation\n");
		return 1;
	}

	LOG("[INFO] Socket Created\n");

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(option), sizeof(option)))
    {
        LOG("[ERROR] Cannot Set socket options\n");
        return 1;
    }
	/*Setting up the sockaddr_in structure */
	addr.sin_family = AF_INET;
	/* Change the below address to our IP addr */
	addr.sin_addr.s_addr = inet_addr("192.168.1.238");//INADDR_ANY;	
	//addr.sin_addr.s_addr = htonl("192.168.1.238");	
	addr.sin_port = htons(PORT);

	if((bind(fd,(struct sockaddr*)&addr, sizeof(addr))) < 0)
	{

		LOG("[ERROR] Cannot bind the socket\n");
		return 1;
	}

	LOG("[INFO] Socket binded\n");

	if(listen(fd,5) < 0)
	{
		LOG("[ERROR] Cannot listen\n");
		return 1;
	}


	int new_fd;

	//while(1)
	//{
		new_fd = accept(fd, (struct sockaddr*)&peer_addr,(socklen_t*)&addrLen);
		if(new_fd < 0)
		{
			LOG("[ERROR] Cannot accept\n");
	//		continue;
			return 1;
		}

		char peer_IP[20] = {0};
		LOG("[INFO] Peer Addr: %s\n",inet_ntop(AF_INET, &peer_addr.sin_addr, peer_IP, sizeof(peer_IP)));

		char readBuffer[1024] = {0};
		int bytesRead;
		size_t payloadLen = 0;
		bytesRead = read(new_fd, &payloadLen, sizeof(size_t));
		if(bytesRead == sizeof(size_t))
		{
			LOG("[INFO] Size of incoming payload: %d\n",payloadLen);
		}	
		else
		{
			LOG("[ERROR] Invalid data\n");
			return 1;
		}
		int i = 0;
		while((bytesRead = read(new_fd, readBuffer+i, 1024)) < payloadLen)
		{
			LOG("[INFO] Number of bytes recvd: %d\n",bytesRead);
			i+=bytesRead;	
		}
		payload_t *payloadptr= (payload_t*)readBuffer;
		LOG("[INFO] Message Recvd\nMessage: %s\nMessageLen: %d\nUSRLED: %d\n",payloadptr->buffer,payloadptr->bufferLen,payloadptr->usrLed_onoff);
	//}
		close(new_fd);

	return 0;
}