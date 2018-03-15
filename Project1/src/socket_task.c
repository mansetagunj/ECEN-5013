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
#include "logger_task.h"
#include "main_task.h"
#include "error_data.h"
#include "common_helper.h"
#include "sensor_common_object.h"
#include "light_sensor_task.h"
#include "temperature_sensor_task.h"

#define SERVER_PORT 	3000
#define SERVER_IP       "127.0.0.1"
#define MAX_CONNECTIONS 5

/**
 * @brief 
 * 
 * @param req_in 
 * @return REMOTE_RESPONSE_T 
 */
REMOTE_RESPONSE_T processRemoteRequest(REMOTE_REQUEST_T req_in);

/**
 * @brief 
 * 
 * @param sigval 
 */
static void timer_handler_sendSTAliveMSG(union sigval sig)
{		
    pthread_mutex_lock(&aliveState_lock);
	aliveStatus[SOCKET_TASK_ID]++;
	pthread_mutex_unlock(&aliveState_lock);
}

/**
 * @brief 
 * 
 * @param server_socket 
 * @return int 
 */
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

	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &(option), sizeof(option)))
    {
        LOG_STDOUT(ERROR "Cannot Set socket options:%s\n",strerror(errno));
        return ERR;
    }
	/*Setting up the sockaddr_in structure */
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = inet_addr(SERVER_IP);	
	addr.sin_addr.s_addr = INADDR_ANY;	//Using local loopback	
	addr.sin_port = htons(SERVER_PORT);

	if((bind(server_socket,(struct sockaddr*)&addr, sizeof(addr))) < 0)
	{
		LOG_STDOUT(ERROR "Cannot bind the socket:%s\n",strerror(errno));
		return ERR;
	}

	LOG_STDOUT(INFO "Socket binded\n");

    if(listen(server_socket,MAX_CONNECTIONS) < 0)
	{
		LOG_STDOUT(ERROR "Cannot listen:%s\n",strerror(errno));
		return ERR;
	}

    LOG_STDOUT(INFO "Socket started listening on IP:%s PORT:%d\n",SERVER_IP,SERVER_PORT);

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


	LOG_STDOUT(INFO "SOCKET TASK INIT COMPLETED\n");
	pthread_barrier_wait(&tasks_barrier);

	DEFINE_LOG_STRUCT(logData,LT_MSG_LOG,SOCKET_TASK_ID);

	while(1)
	{
		POST_MESSAGE_LOGTASK(&logData,"Accepting connections...\n");
		accepted_socket = accept(server_socket, (struct sockaddr*)&peer_addr,(socklen_t*)&addrLen);
		if(accepted_socket < 0)
		{
			LOG_STDOUT(ERROR "Cannot accept:%s\n",strerror(errno));
			POST_MESSAGE_LOGTASK(&logData,ERROR "Cannot accept:%s\n",strerror(errno));
			continue;
		}

		char peer_IP[20] = {0};
		POST_MESSAGE_LOGTASK(&logData,"Conn accepted. Peer Add: %s\n",inet_ntop(AF_INET, &peer_addr.sin_addr, peer_IP, sizeof(peer_IP)));
		LOG_STDOUT(INFO "Connection accepted from peer Addr: %s\n",inet_ntop(AF_INET, &peer_addr.sin_addr, peer_IP, sizeof(peer_IP)));

		while(1)
		{
			REMOTE_REQUEST_T req_in = {0};
			REMOTE_RESPONSE_T rsp_out = {0};
			int nbytes = 0;
			do{
				nbytes = recv(accepted_socket, (((char*)&(req_in))+nbytes), sizeof(req_in), 0);
			}while(nbytes < sizeof(req_in) && nbytes != -1);

			LOG_STDOUT(INFO "--CLIENT REQUEST: bytes:%d ID:%d\n",nbytes,req_in.request_id);
			rsp_out = processRemoteRequest(req_in);
			
			if(rsp_out.rsp_id == CONN_CLOSE_RSP) 
				{break;}
		
			nbytes = send(accepted_socket , (char*)&rsp_out , sizeof(rsp_out), 0 );
			if(nbytes < sizeof(rsp_out))
			{
				LOG_STDOUT(ERROR "Cannot send complete data\n");
				break;
				//return 1;
			}

			LOG_STDOUT(INFO "Number of bytes sent: %d\n",nbytes);
		
		}

        /* Create a new thread to handle the connection and go back to accepting */
        close(accepted_socket);
		LOG_STDOUT(INFO "Socket Closed\n");
		POST_MESSAGE_LOGTASK(&logData,INFO "Socket Closed\n");
        /* Think of a mechanism to close this socket task as there is a while(1) loop */

    }

}

REMOTE_RESPONSE_T processRemoteRequest(REMOTE_REQUEST_T req_in)
{
	REMOTE_RESPONSE_T rsp_out = {0};
	LOG_STDOUT(INFO "IN process msg\n");
	switch(req_in.request_id)
    {
        case(GET_FUNC):
			rsp_out.rsp_id=GET_FUNC;
            strncpy(rsp_out.metadata,"GET_TEMP DAY_NIGHT\n", sizeof(rsp_out.metadata));
            break;
        case(GET_TEMP_C):
			rsp_out.rsp_id=GET_TEMP_C;
			rsp_out.data.floatingData = getTempTask_temperature();
            LOG_STDOUT(INFO "REMOTE REQUEST GET_TEMP_C:%f\n",rsp_out.data.floatingData);
            break;
        case(GET_TEMP_F):
			rsp_out.rsp_id=GET_TEMP_F;
			rsp_out.data.floatingData = getTempTask_temperature();
            LOG_STDOUT(INFO "REMOTE REQUEST GET_TEMP_F\n");
            break;
        case(GET_TEMP_K):
			rsp_out.rsp_id=GET_TEMP_K;
			rsp_out.data.floatingData = getTempTask_temperature();
            LOG_STDOUT(INFO "REMOTE REQUEST GET_TEMP_K\n");
            break;
        case(GET_LUX):
			rsp_out.rsp_id=GET_LUX;
			rsp_out.data.floatingData = 1.0;
            LOG_STDOUT(INFO "REMOTE REQUEST GET_LUX\n");
            break;
        case(GET_DAY_NIGHT):
			rsp_out.rsp_id=GET_DAY_NIGHT;
			rsp_out.data.isNight = getLightTask_state();
            LOG_STDOUT(INFO "REMOTE REQUEST GET_DAY_NIGHT\n");
            break;
    	case(CONN_CLOSE_REQ):
			rsp_out.rsp_id=CONN_CLOSE_RSP;
			LOG_STDOUT(INFO "REMOTE REQUEST CLOSE CONNECTION\n");
             break;
        default:
			LOG_STDOUT(INFO "REMOTE REQUEST INVALID\n");
            break;

    }
	return rsp_out;
}