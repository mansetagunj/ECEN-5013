#include <sys/time.h>
#include <time.h>
#include <string.h>


#define GET_TIMEOFDAY(x,y)	gettimeofday(x,y)	//syscall(__sys_gettimeofday,x,y)

int get_time_string(char *timeString)
{
	struct timeval tv;
	struct tm* ptm;
	char time_string[40] = {0};
	long milliseconds;

 	/* Obtain the time of day using the system call */
	unsigned long ret = GET_TIMEOFDAY(&tv,NULL);
	if(ret != 0)
	{
		memset(timeString,0,1);
		return 1;
	}
	ptm = localtime (&tv.tv_sec);
	/* Format the date and time. */
	//strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	strftime (time_string, sizeof (time_string), "%X", ptm);
    memcpy(timeString,time_string,40);
    
    return 0;
}
