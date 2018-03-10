/**
 * @brief 
 * 
 * @file socket_task.c
 * @author Gunj Manseta
 * @date 2018-03-09
 */

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
		
#include "socket_task.h"
#include "error_data.h"

#define SERVER_PORT 	2000
#define SERVER_IP       "127.0.0.1"
#define MAX_CONNECTIONS 5

int socket_task_init(int server_socket)
{
    int option = 1;
	struct sockaddr_in addr;
	
	if((server_socket = socket(AF_INET,SOCK_STREAM,0)) == 0)
	{
		LOG_STDOUT(ERROR "Socket Creation:%s\n",strerror(errno));
		return ERR;
	}

	LOG_STDOUT(INFO "Socket Created\n");

	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(option), sizeof(option)))
    {
        LOG_STDOUT(ERROR "Cannot Set socket options\n");
        return ERR;
    }
	/*Setting up the sockaddr_in structure */
	addr.sin_family = AF_INET;\
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);	
	//addr.sin_addr.s_addr = INADDR_ANY;	//Using local loopback	
	addr.sin_port = htons(SERVER_PORT);

	if((bind(server_socket,(struct sockaddr*)&addr, sizeof(addr))) < 0)
	{
		LOG_STDOUT(ERROR "Cannot bind the socket\n");
		return ERR;
	}

	LOG_STDOUT(INFO "Socket binded\n");

    if(listen(server_socket,MAX_CONNECTIONS) < 0)
	{
		LOG_STDOUT(ERROR "Cannot listen\n");
		return ERR;
	}

    LOG_STDOUT(INFO "Socket started listening\n");

    return server_socket;
}

void* socket_task_callback(void* threadparam)
{
    int server_socket,accepted_socket, option = 1;
    struct sockaddr_in peer_addr;
	int addrLen = sizeof(peer_addr);
    LOG_STDOUT(INFO "SOCKET TASK STARTED\n");

    server_socket  = socket_task_init(server_socket);
    if(ERR == server_socket)
    {
        LOG_STDOUT(ERROR "Socket task init failed.\n");
        exit(ERR);
    }

	while(1)
	{
		accepted_socket = accept(server_socket, (struct sockaddr*)&peer_addr,(socklen_t*)&addrLen);
		if(accepted_socket < 0)
		{
			LOG_STDOUT(ERROR "Cannot accept\n");
			continue;
		}

		char peer_IP[20] = {0};
		LOG_STDOUT(INFO "Connection accepted from peer Addr: %s\n",inet_ntop(AF_INET, &peer_addr.sin_addr, peer_IP, sizeof(peer_IP)));


        /* Create a threade to handle the connection and go back to accepting */
        close(accepted_socket);
        /* Think of a mechanism to close this socket task as there is a while(1) loop */

    }

}