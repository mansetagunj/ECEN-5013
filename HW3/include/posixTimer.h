#ifndef __POSIX_TIMER_H__
#define __POSIX_TIMER_H__


#include <time.h>
#include <linux/types.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


#define MICROSEC	(1000000)

int register_timer(timer_t *timer_id, void (*timer_handler)(union sigval), void *handlerArgs);

int start_timer(timer_t timer_id , uint32_t time_usec, uint8_t oneshot);

int stop_timer(timer_t timer_id);

int delete_timer(timer_t timer_id);

#endif
