#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>

#define MAX_BUFFER_LEN (1023)
#define MIN_BUFFER_LEN (1)

SYSCALL_DEFINE3(g_ksort,
                const int __user *, in_p_buffer,
                int               , in_bufferLen,
                int __user *      , in_out_p_buffer)
{
	int i,j;
	unsigned long in_bufferLen_bytes;	
	int *p_kbuffer = NULL;
	
	printk(KERN_INFO "Entered g_ksort\n");

	if(in_p_buffer == NULL || in_out_p_buffer == NULL)
	{
		printk(KERN_ERR "[NULL ERROR] NULL POINTER AS PARAM.\n");
		return -EFAULT;
	}
	if((in_bufferLen < MIN_BUFFER_LEN) || (in_bufferLen > MAX_BUFFER_LEN))
	{
		printk(KERN_INFO "[INVALID] BUFFER SIZE INVALID - ACCEPTED SIZE = [1,1023]. BUFFERSIZE:%d\n",in_bufferLen);
		return -EINVAL;
	}

	in_bufferLen_bytes = (sizeof(int)*in_bufferLen);


	//(type,addr,size) - type -> VERIFY_WRITE & VERIFY_READ
	//access_ok returns 0 on fail and non-zero on sucess
	if((!access_ok(VERIFY_WRITE,in_p_buffer,in_bufferLen)) ||
	   (!access_ok(VERIFY_WRITE,in_out_p_buffer,in_bufferLen))
	)
	{
		printk(KERN_ERR "[PERMISSION ERROR] ACCESS_OK FAILED FOR USER SPACE BUFFER\n");
		return -EFAULT;
	}
	

	p_kbuffer = (int*)kmalloc(in_bufferLen_bytes, GFP_KERNEL);
	if(!p_kbuffer)
	{	
		printk(KERN_ERR "[MEM ERROR] KMALLOC FAILED FOR KERNEL SPACE BUFFER\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "Buffer of Length %d\n",in_bufferLen);
	
	//copies the data from user space to the kernel space
	//(kernel_dst,user_src,len) - (to,from,len)
	if(copy_from_user(p_kbuffer, in_p_buffer, in_bufferLen_bytes))
	{	
		printk(KERN_ERR "[ERROR] COPY_FROM_USER FAILED\n");
		kfree(p_kbuffer);
		return -EFAULT;
	}
	
	printk(KERN_INFO "SORTING STARTED\n");
	
	for (i = 0; i < (in_bufferLen-1); i++)      
 	{
		for (j = 0; j < (in_bufferLen-i-1); j++)
		{ 
			if (*(p_kbuffer+j) > *(p_kbuffer+j+1))
			{
				int temp = *(p_kbuffer+j);
				*(p_kbuffer+j) = *(p_kbuffer+j+1);
				*(p_kbuffer+j+1) = temp;
			}
		}
	}
	printk(KERN_INFO "SORTING COMPLETED\n");

	
	//copies the data from kernel space to user space
	//(user_dst, kernel_src, len) - (to,from,len)
	if(copy_to_user(in_out_p_buffer, p_kbuffer, in_bufferLen_bytes))
	{
		printk(KERN_ERR "[ERROR] COPY_TO_USER FAILED\n");	
		kfree(p_kbuffer);
		return -EFAULT;
	}

	kfree(p_kbuffer);

	printk(KERN_INFO "Exiting g_ksort\n");

	return 0;
}
