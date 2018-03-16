
#include "posixTimer.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

int register_timer(timer_t *timer_id, void (*timer_handler)(union sigval), void *handlerArgs)
{

	if(NULL == timer_id)
		return -1;
		
	struct sigevent sige;

	/*SIGEV_THREAD will call the handler as if it was a new thread */
	sige.sigev_notify = SIGEV_THREAD;
	sige.sigev_notify_function = timer_handler;
//	sige.sigev_value.sival_ptr = timer_id;
	sige.sigev_value.sival_ptr = handlerArgs;
	sige.sigev_notify_attributes = NULL;

	int ret = timer_create(CLOCK_REALTIME, &sige, timer_id);
	
	return ret;
}

int start_timer(timer_t timer_id , uint64_t time_usec, uint8_t oneshot)
{
	if(NULL == timer_id)
		return -1;
		
	struct itimerspec ts;
	
	ts.it_value.tv_sec = time_usec / MICROSEC;
	ts.it_value.tv_nsec = (time_usec % MICROSEC) * 1000;
	if(1 == oneshot)
	{
		ts.it_interval.tv_sec = 0;
		ts.it_interval.tv_nsec = 0;
	}
	else
	{
		ts.it_interval.tv_sec = ts.it_value.tv_sec;
		ts.it_interval.tv_nsec = ts.it_value.tv_nsec;
	}

	int ret = timer_settime(timer_id, 0, &ts, 0);
	
	return ret;
}

int stop_timer(timer_t timer_id)
{
	if(NULL == timer_id)
		return -1;
		
	struct itimerspec ts;
	
	ts.it_value.tv_sec = 0;
	ts.it_value.tv_nsec = 0;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;

	int ret = timer_settime(timer_id, 0, &ts, 0);
	
	return ret;
}


int delete_timer(timer_t timer_id)
{
	if(NULL == timer_id)
		return -1;
	
	int ret = timer_delete(timer_id);

	return ret;


}
