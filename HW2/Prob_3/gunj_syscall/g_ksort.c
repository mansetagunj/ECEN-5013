#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>

SYSCALL_DEFINE3(g_ksort,		\
		int __user, in_p_buffer,	\
		int __user, in_bufferLen,	\
		int __user, in_out_p_buffer)	\
{

	//put in check for the pointers

	printk(KERN_INFO "Entered g_ksort\n");

	if(in_bufferLen > 255)
	{
		printk(KERN_DEBUG "BUFFER SIZE GREATER THAN 255\n");
		return -EINVAL;
	}

	//(type,addr,size) - type -> VERIFY_WRITE & VERIFY_READ
	//return 0 on fail and non-zero on sucess
	if((!access_ok(VERIFY_WRITE,in_p_buffer,in_bufferLen)) ||
	   (!access_ok(VERIFY_WRITE,in_out_p_buffer,in_bufferLen))
	)
	{
		printk(KERN_DEBUG "ACCESS OK FAILED FOR USER SPACE BUFFER\n");
		return -EFAULT;
	}
	

	int *p_kbuffer = (int*)kmalloc(sizeof(int)*in_bufferLen, GFP_KERNEL);
	if(!p_kbuffer)
		return -ENOMEM;

	printk(KERN_INFO "Buffer of Length %d\n",in_bufferLen);
	
	//copies the data from user space to the kernel space
	//(kernel_dst,user_src,len) - (to,from,len)
	if(copy_from_user(p_kbuffer, in_p_buffer, in_bufferLen))
	{
		kfree(p_kbuffer);
		return -EFAULT;
	}

	printk(KERN_INFO "Placeholder  for sort algo");

	
	//copies the data from kernel space to user space
	//(user_dst, kernel_src, len) - (to,from,len)
	if(copy_to_user(in_out_p_buffer, p_kbuffer, in_bufferLen))
	{
		kfree(p_kbuffer);
		return -EFAULT;
	}

	printk(KERN_INFO "Exiting g_ksort\n");

	return 0;
}
