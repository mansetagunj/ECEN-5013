#include <linux/syscalls.h>
#include <linux/kernel.h>

SYSCALL_DEFINE3(g_ksort,
                const int __user *, in_p_buffer,
                int               , in_bufferLen,
                int __user *      , in_out_p_buffer);



