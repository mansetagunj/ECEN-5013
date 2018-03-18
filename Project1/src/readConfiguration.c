/**
 * @brief 
 * 
 * @file readConfiguration.c
 * @author Gunj Manseta
 * @date 2018-03-17
 */


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "readConfiguration.h"

#define CONFIG_DATA_NUM 3

#define CONFIG_FILE "configuration.dat"

typedef enum
{
    LOG_PATH_STRING,
    TASK_SETUP_TIME_SEC_UINT8,
    TASK_ALIVE_TIMEOUT_SEC_UINT8
}CONFIG_DATA_INDEX;

static void* configurationData[CONFIG_DATA_NUM] = {0};

char* configdata_getLogpath()
{
    return ((char*)configurationData[LOG_PATH_STRING]);
}

uint32_t configdata_getSetupTime()
{
    return (*((uint32_t*)configurationData[TASK_SETUP_TIME_SEC_UINT8]));
}
uint32_t configdata_getAliveTimeout()
{
    return (*((uint32_t*)configurationData[TASK_ALIVE_TIMEOUT_SEC_UINT8]));
}

int configdata_setup()
{
    FILE *fp;
    fp = fopen(CONFIG_FILE, "r");
    if(NULL ==fp)
        return -1;
    
    configurationData[LOG_PATH_STRING] = (char*)malloc(sizeof(char)*20);
    configurationData[TASK_SETUP_TIME_SEC_UINT8] = (uint32_t*)malloc(sizeof(uint32_t));
    configurationData[TASK_ALIVE_TIMEOUT_SEC_UINT8] = (uint32_t*)malloc(sizeof(uint32_t));

    size_t readBytes = fscanf(fp,"%s %u %u",(char*)configurationData[LOG_PATH_STRING],(uint32_t*)configurationData[TASK_SETUP_TIME_SEC_UINT8], (uint32_t*)configurationData[TASK_ALIVE_TIMEOUT_SEC_UINT8]);
    
    #ifdef SELF_TEST
    printf("PATH: %s\n",(char*)configurationData[LOG_PATH_STRING]);
    printf("SETUP: %u\n",*(uint32_t*)configurationData[TASK_SETUP_TIME_SEC_UINT8]);
    printf("TO: %u\n",*(uint32_t*)configurationData[TASK_ALIVE_TIMEOUT_SEC_UINT8]);
    #endif

    return 0;
}

void configdata_flush()
{
    for(int i = 0; i <CONFIG_DATA_NUM ; i ++)
    {
        free(configurationData[i]);
        configurationData[i] = NULL;
    }
}

#ifdef SELF_TEST
int main()
{
    int ret = configdata_setup();
    if(ret) return ret;

    printf("From func: %s\n",configdata_getLogpath());
    printf("From func: %u\n",configdata_getSetupTime());
    printf("From func: %u\n",configdata_getAliveTimeout());

    configdata_flush();
    return 0;
}
#endif