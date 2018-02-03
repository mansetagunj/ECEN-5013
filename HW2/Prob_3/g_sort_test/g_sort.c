#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include "g_sort.h"

#define __sys_g_ksort	(385)	// 385 is the syscall number

unsigned long g_sort(const int *buffer, int bufferLen, int *sortedBuffer)
{
	if(!buffer || !sortedBuffer)
		return -1;

	return syscall(__sys_g_ksort, buffer, bufferLen, sortedBuffer);
         
}
