/*
 * @File my_signals.h
 *
 * @Created on: 18-Feb-2018
 * @Author: Gunj Manseta
 */
 
#ifndef __MY_SIGNALS_H__
#define __MY_SIGNALS_H__

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

int register_signalHandler(struct sigaction *sa, void (*handler)(int), REG_SIGNAL_FLAG_t signalMask);

#endif
