#include <stdio.h>

#include "my_signals.h"
#include "log_macros.h"

int register_signalHandler(struct sigaction *sa, void (*handler)(int), REG_SIGNAL_FLAG_t signalMask)
{
	sa->sa_handler = handler;

	sa->sa_flags = SA_RESTART;

	sigfillset(&sa->sa_mask);

	int ret_error = 0;
	
	if ((signalMask & REG_SIG_USR1) && sigaction(SIGUSR1, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGUSR1.\n");
	}

	if ((signalMask & REG_SIG_USR2) && sigaction(SIGUSR2, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGUSR2.\n");
	}
	
	if ((signalMask & REG_SIG_INT) && sigaction(SIGINT, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGINT.\n");
	}
	
	if ((signalMask & REG_SIG_TSTP) && sigaction(SIGTERM, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGTERM.\n");
	}
	
	if ((signalMask & REG_SIG_TSTP) && sigaction(SIGTSTP, sa, NULL) == -1) 
	{
		ret_error++;
		PRINT_THREAD_IDENTIFIER();
		printf("Cannot handle SIGTSTOP.\n");
	}

	return ret_error;
}
