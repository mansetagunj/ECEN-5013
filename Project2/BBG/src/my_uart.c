/**
 * @brief 
 * 
 * @file my_uart.c
 * @author Gunj Manseta
 * @date 2018-04-23
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "my_uart.h"
#include "communication_object.h"

#define BAUD_115200     B115200
#define BAUD_9921600    B921600

#define BAUDRATE    BAUD_9921600

const char *const COMPORT[5] = {"","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4"};

/* These functions are specific to the ttyS4*/
#define UART_DEV "/dev/ttyS4" //Beaglebone Green serial port


//caching the old tio config to keep it back as it was
static struct termios old_tio_config;

static int32_t opened = 0;
static int internal_fd = -1;

UART_FD_T UART_Open(COM_PORT com_port)
{
    //Not supporing other com ports as I dont have time to handle open close and release for all the com ports
    if(com_port != COM_PORT4)
        return -1;

    if(opened > 0 && internal_fd != -1)
    {
        if(opened < 64)
            return internal_fd;
        else
            return -1;
    }

    int fd;
    struct termios tio_config;
    char buf[255];

    fd = open(COMPORT[COM_PORT4], O_RDWR | O_NOCTTY | O_SYNC );
    if (fd < 0) 
    { 
        perror(UART_DEV); 
        return -1;
        //exit(-1); 
    }

    tcgetattr(fd, &old_tio_config);

    bzero(&tio_config, sizeof(tio_config)); 

    tio_config.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    cfmakeraw(&tio_config);

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tio_config);

    internal_fd = fd;
    opened++;
    return fd;
}

void UART_Release(UART_FD_T fd)
{
    tcsetattr(fd, TCSANOW, &old_tio_config);
    close(fd);
}

void UART_Close(UART_FD_T fd)
{
    opened--;
    if(opened > 0)
    {
        //dummy close
    }
    else
    {
        if(fd == internal_fd)
            UART_Release(fd);
    }
}

int32_t UART_putchar(char c)
{
    return write(internal_fd, &c, 1);    
}

int32_t UART_putRAW(void *object, size_t len)
{
    int32_t ret = 0, retry = 0, i =0;
    do
    {
        ret = write(internal_fd, object+i, len-i);
        i += ret;
        retry++;
    }while(ret > -1 && i < len && retry < 16);
    //printf("I:%d RET: %dRETRY: %d\n",i, ret, retry);
    return ret;
}

int32_t UART_putstr(const char* str)
{
    return UART_putRAW((void*)str, strlen(str));
}

int32_t UART_printf(const char *fmt, ...)
{
    //this function is not needed 
}

int32_t UART_read(void *object, size_t len)
{
    int ret = 0, retry = 0, i =0;
    do
    {
        ret = read(internal_fd, object+i, len-i);
        i +=ret;
        retry++;
    }while(ret > -1 && i < len && retry < 16);

    //printf("i = %d RET: %d RETRY: %d\n",i, ret, retry);
    return ret;
}

#ifdef SELF_TEST
#include <errno.h>

int main()
{
    printf("SIZE: %u\n",sizeof(COMM_MSG_T));
    int fd = UART_Open(COM_PORT4);
    if(fd == -1)
    {
        printf("UART open ERROR");
        return fd;
    }
    int ret = 0;
    #ifdef LOOPBACK
	COMM_MSG_T comm_msg = 
    {
        .ID = HEARTBEAT | (0x55<<24),
        .payload = "SELF CHECK",
    };
    comm_msg.checksum = getCheckSum(&comm_msg);

	
	printf("SIZEOF: %u\n",sizeof(comm_msg));
    printf("Sending:\nBOARDID: 0x%01x ID:%u, PAYLOAD:%s, CHECKSUM:%u\n",GET_BOARD_UID_FROM_LOG_ID(comm_msg.ID),GET_LOG_ID_FROM_LOG_ID(comm_msg.ID),comm_msg.payload,comm_msg.checksum);
    
    ret = UART_putRAW(&comm_msg,sizeof(comm_msg));
    if(ret == -1)
        printf("Serial Write Error: %s\n",strerror(errno));
    else
        printf("Bytes sent: %d\n",ret);

    #endif

    COMM_MSG_T recv_comm_msg = {0};
    
    #ifndef LOOPBACK
    while(1)
    {
    #endif
    ret = UART_read(&recv_comm_msg,sizeof(recv_comm_msg));
    if(ret == -1)
        printf("Serial Read Error: %s\n",strerror(errno));
    else
        printf("Bytes recvd: %d\n",ret);
        
    uint16_t check = getCheckSum(&recv_comm_msg);
    printf("Recvd:\nBOARDID: 0x%01x ID:%u, PAYLOAD:%s,\nGOT_CHECKSUM:%u ACTUAL_CHECKSUM:%u\n",GET_BOARD_UID_FROM_LOG_ID(recv_comm_msg.ID),GET_LOG_ID_FROM_LOG_ID(recv_comm_msg.ID),recv_comm_msg.payload,recv_comm_msg.checksum,check);
    #ifndef LOOPBACK
    }
    #endif
    tcflush(fd, TCIFLUSH);
    UART_Close(fd);

	return 0;
}

#endif