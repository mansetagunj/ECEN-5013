#include <linux/syscalls.h>
#include <linux/kernel.h>

SYSCALL_DEFINE3(g_ksort,		\
		int, in_p_buffer,	\
		int, in_bufferLen,	\
		int, in_out_p_buffer);
