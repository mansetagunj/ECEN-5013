/**
 * @brief 
 * 
 * @file my_signals.c
 * @author Gunj Manseta
 * @date 2018-03-18
 */

#include "my_signals.h"
#include "error_data.h"

int register_signalHandler(struct sigaction *sa, void (*handler)(int), REG_SIGNAL_FLAG_t signalMask)
{
	sa->sa_handler = handler;

	sa->sa_flags = SA_RESTART;

	sigfillset(&sa->sa_mask);

	int ret_error = 0;
	
	if ((signalMask & REG_SIG_USR1) && sigaction(SIGUSR1, sa, NULL) == -1) 
	{
		ret_error++;
		LOG_STDOUT(ERROR "Cannot handle SIGUSR1.\n");
	}

	if ((signalMask & REG_SIG_USR2) && sigaction(SIGUSR2, sa, NULL) == -1) 
	{
		ret_error++;
		LOG_STDOUT(ERROR "Cannot handle SIGUSR2.\n");
	}
	
	if ((signalMask & REG_SIG_INT) && sigaction(SIGINT, sa, NULL) == -1) 
	{
		ret_error++;
		LOG_STDOUT(ERROR "Cannot handle SIGINT.\n");
	}
	
	if ((signalMask & REG_SIG_TSTP) && sigaction(SIGTERM, sa, NULL) == -1) 
	{
		ret_error++;
		LOG_STDOUT(ERROR "Cannot handle SIGTERM.\n");
	}
	
	if ((signalMask & REG_SIG_TSTP) && sigaction(SIGTSTP, sa, NULL) == -1) 
	{
		ret_error++;
		LOG_STDOUT(ERROR "Cannot handle SIGTSTOP.\n");
	}

	return ret_error;
}
