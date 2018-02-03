#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "g_sort.h"

#define BUFFER_LEN	(20)
#ifdef BUILD_TIME
#define RAND_SEED	(BUILD_TIME)
#else
#define RAND_SEED	(40)
#endif

int main()
{
    printf("Process ID:\t %d\n", getpid());
    printf("User ID:\t %u\n", getuid());     
    srand(RAND_SEED);
         
    int *buffer = (int*)malloc(sizeof(int)*BUFFER_LEN);
    if(buffer == NULL)
    {
	printf("Malloc Failed. Try Again\n");
	return (-1);
    }

    printf("Buffer presort.\n");
    for(int i = 0; i < BUFFER_LEN; i++)
    {
	buffer[i] = rand()%50;
	printf("[%d] ",buffer[i]);
    }
    printf("\n");
    int *outBuffer = (int*)malloc(sizeof(int)*BUFFER_LEN);
    if(outBuffer == NULL)
    {
	printf("Malloc Failed. Try Again\n");
	free(buffer);
	return (-1);
    }
    memset(outBuffer,0,10);	

    
    printf("Invoking 'g_ksort' system call\n");

    long ret_status = g_sort(buffer,BUFFER_LEN,outBuffer);
         
    if(ret_status == 0) 
    {
	printf("System call  executed correctly. Use dmesg for details.\n");
    
	printf("Buffer post sort.\n");
	for(int i = 0; i < BUFFER_LEN; i++)
	{
		printf("[%d] ",outBuffer[i]);
	}
	printf("\n");		
    }	
    else 
         printf("System call  did not execute as expected. Use dmesg to check. Error:%ld:%s\n",ret_status,strerror(ret_status));
          
     return (0);
}
