/**
 * @brief 
 * 
 * @file my_signals.h
 * @author Gunj Manseta
 * @date 2018-03-18
 */
 
#ifndef MY_SIGNALS_H
#define MY_SIGNALS_H

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
		
#define REG_SIG_USR1	(1<<0)
#define REG_SIG_USR2	(1<<1)
#define REG_SIG_INT		(1<<2)
#define REG_SIG_TERM	(1<<3)
#define REG_SIG_TSTP	(1<<4)
#define REG_SIG_ALL		(0x1F)

typedef uint8_t REG_SIGNAL_FLAG_t ;

/**
 * @brief Register asignal handler for specific signal masks
 * 
 * @param sa 
 * @param handler 
 * @param signalMask 
 * @return int 
 */
int register_signalHandler(struct sigaction *sa, void (*handler)(int), REG_SIGNAL_FLAG_t signalMask);



#endif
