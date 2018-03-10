#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "my_time.h"
#include "error_data.h"

#define GET_TIMEOFDAY(x,y)	gettimeofday(x,y)	//syscall(__sys_gettimeofday,x,y)

int get_time_string(char *timeString, const int len)
{
	struct timeval tv;
	//struct tm* ptm;
	char time_string[20] = {0};

 	/* Obtain the time of day using the system call */
	unsigned long ret = GET_TIMEOFDAY(&tv,NULL);
	if(ret != 0)
	{
		memset(timeString,0,len);
		return ERR;
	}
	snprintf(time_string,sizeof(time_string),"%ld.%ld",tv.tv_sec,tv.tv_usec);
	//ptm = localtime (&tv.tv_sec);
	/* Format the date and time. */
	//strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	//strftime (time_string, sizeof (time_string), "%X", ptm);
    memcpy(timeString,time_string, len);
    
    return SUCCESS;
}
