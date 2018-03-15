#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "sensor_common_object.h"

//#define PORT 3000
//#define IP "127.0.0.1"
//#define IP "192.168.1.238"
#define LOG(format, ...) printf(format, ##__VA_ARGS__)

void printResponse(REMOTE_RESPONSE_T rsp);

int main()
{
    struct sockaddr_in addr, server_addr = {0};
    uint16_t PORT = 3000;
    char IP[20] = "127.0.0.1";

    int client_socket = 0;
    REMOTE_REQUEST_T req = {0};
    REMOTE_RESPONSE_T rsp = {0};

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG("[ERROR] Socket creation\n");
        return -1;
    }
  
    LOG("[INFO] Socket Created\n");

    //memset(&server_addr, 0, sizeof(server_addr));
  
    server_addr.sin_family = AF_INET;

    LOG("***CLIENT APPLICATION***\n");
    LOG("Default IP:%s PORT%u\n",IP,PORT);
    LOG("Enter new IP and Port?(y/n)->");
    char ans;
    scanf(" %c",&ans);
    if(ans == 'y' || ans == 'Y')
    {
        LOG("Enter Port number ->");
        scanf(" %hu",&PORT);
        LOG("Enter IP addr ->");
        scanf("%s",IP);
        //fgets(IP, 20, stdin);
    }

    server_addr.sin_port = htons(PORT);
      
    /* We need this to convert the IP ADDR in proper format */
    if(inet_pton(AF_INET, IP, &server_addr.sin_addr)<=0) 
    {
        LOG("[ERROR] Invalid address\n");
        return -1;
    }

    LOG("Continue?(y/n)->");
    scanf(" %c",&ans);
    if(ans == 'n' || ans == 'N')
        exit(0);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        LOG("[ERROR] Connection Failed \n");
        return -1;
    }
    int i = 0, nbytes;
    int continue_flag = 1;
    do{

        req.request_id = GET_TEMP_C + i%5;
        /*Sending the payload */
        nbytes = send(client_socket , (char*)&req , sizeof(req), 0 );
        if(nbytes < sizeof(req))
        {
            LOG("[ERROR] Cannot send complete data\n");
            return 1;
        }

        LOG("[INFO] Number of bytes sent: %d\n",nbytes);
        
        nbytes=0;
        do{
            nbytes = recv(client_socket, (((char*)&(rsp))+nbytes), sizeof(rsp), 0);
        }while(nbytes < sizeof(rsp) && nbytes != -1);

        LOG("[INFO] Receivced bytes: %d\n",nbytes);

        LOG("\n***SERVER RESPONSE***\n");
        printResponse(rsp);
        LOG("\n*********************\n");

        LOG("Continue?(y/n)->");
        scanf(" %c",&ans);
        if(ans == 'n' || ans == 'N')
            continue_flag = 0;
        
    }while(continue_flag);

    req.request_id = CONN_CLOSE_REQ;
    nbytes = send(client_socket , (char*)&req , sizeof(req), 0 );
    if(nbytes < sizeof(req))
    {
        LOG("[ERROR] Cannot send complete data\n");
        return 1;
    }

    LOG("[INFO] Number of bytes sent: %d\n",nbytes);

    close(client_socket);

    LOG("[INFO] Connection closed;\n");
    return 0;
}

void printResponse(REMOTE_RESPONSE_T rsp)
{
    switch(rsp.rsp_id)
    {
        case(GET_FUNC):
            LOG("%s",rsp.metadata);
            break;
        case(GET_TEMP_C):
            LOG("degree C : %0.3f\n",rsp.data.floatingData);
            break;
        case(GET_TEMP_F):
            LOG("degree K : %0.3f\n",rsp.data.floatingData);
            break;
        case(GET_TEMP_K):
            LOG("degree F : %0.3f\n",rsp.data.floatingData);
            break;
        case(GET_LUX):
            LOG("LUX : %0.3f\n",rsp.data.floatingData);
            break;
        case(GET_DAY_NIGHT):
            LOG("It is %s now\n",((rsp.data.isNight == 0) ? "DAY" : "NIGHT"));
            break;
        // case(CONN_CLOSE_RSP):
        //     break;
        default:
            break;

    }
}