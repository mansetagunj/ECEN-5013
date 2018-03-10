/*
 * @File posixTimer.h
 *
 * @Created on: 18-Feb-2018
 * @Author: Gunj Manseta
 */

#ifndef POSIX_TIMER_H
#define POSIX_TIMER_H


#include <time.h>
#include <linux/types.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


#define MICROSEC	(1000000)

/**
 * @brief REgsiter the timer handler 
 * 
 * @param timer_id 
 * @param timer_handler 
 * @param handlerArgs 
 * @return int 
 */
int register_timer(timer_t *timer_id, void (*timer_handler)(union sigval), void *handlerArgs);

/**
 * @brief Starts the timer 
 * 
 * @param timer_id 
 * @param time_usec 
 * @param oneshot 
 * @return int 
 */
int start_timer(timer_t timer_id , uint32_t time_usec, uint8_t oneshot);

/**
 * @brief Stops the timer 
 * 
 * @param timer_id 
 * @return int 
 */
int stop_timer(timer_t timer_id);

/**
 * @brief Destroys the timer 
 * 
 * @param timer_id 
 * @return int 
 */
int delete_timer(timer_t timer_id);

#endif
