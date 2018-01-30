#include <linux/syscalls.h>
#include <linux/kernel.h>

SYSCALL_DEFINE3(g_ksort,		\
		int, in_p_buffer,	\
		int, in_bufferLen,	\
		int, in_out_p_buffer)	\
{

	//put in check for the pointers

	printk(KERN_INFO "Entered g_ksort\n");

	int *p_kbuffer = (int*)kmalloc(sizeof(int)*in_bufferLen, GFP_KERNEL);
	

	//copies the data from user space to the kernel space
	//(user_src,kernel_dst,len)
	if(copy_from_user(in_p_buffer, p_kbuffer, in_bufferLen))
	{
		kfree(p_kbuffer);
		return -EFAULT;
	}

	printk(KERN_INFO "Placeholder  for sort algo");

	
	//copies the data from kernel space to user space
	//(user_dst, kernel_src, len)
	if(copy_to_user(in_out_p_buffer, p_kbuffer, in_bufferLen))
	{
		kfree(p_kbuffer);
		return -EFAULT;
	}

	printk(KERN_INFO "Exiting g_ksort\n");

	return 0;
}
