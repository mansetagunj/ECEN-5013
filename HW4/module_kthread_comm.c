#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/kthread.h>	
#include <linux/delay.h>
#include <linux/kfifo.h>
#include <linux/sched.h>

#define MY_KFIFO_SIZE	(16)
#define MY_KFIFO_NAME	mykfifo
#define MY_KFIFO_NAME_P	&mykfifo

static DEFINE_MUTEX(fifo_lock);

static int dataProducedCount = 0;
static int dataConsumedCount = 0;

struct task_struct *producer_task;
struct task_struct *consumer_task;

static DECLARE_KFIFO(MY_KFIFO_NAME,struct task_struct*,MY_KFIFO_SIZE);	

static unsigned long stimeInterval= 5;
module_param(stimeInterval, ulong, S_IRUGO | S_IWUSR);
	
int producer_callback(void *params)
{
	printk(KERN_INFO "From %s\n",__FUNCTION__);

	while(!kthread_should_stop())
	{		
		/* Lock the mutex*/
		if (mutex_lock_interruptible(&fifo_lock))
		{
			printk(KERN_ERR "Cannot get the lock\n");
			//return -1;
			return -ERESTARTSYS;
		}
		/* Push the data into kfifo*/
		if(0 == kfifo_put(MY_KFIFO_NAME_P, current))
			printk(KERN_INFO "KFIFO FULL\n");
		else
		{
			//printk(KERN_INFO "Process pushed id: %d\n",current->pid);
		}

		/* Unlock the mutex*/
		mutex_unlock(&fifo_lock);

		/* Signal the condition variable */

		dataProducedCount++;
		ssleep(stimeInterval);
	}

	printk(KERN_INFO "%s is terminated\n",__FUNCTION__);	

	return dataProducedCount;
}

int consumer_callback(void *params)
{
	struct task_struct *fifoData;
	printk(KERN_INFO "From %s\n",__FUNCTION__);

	while(!kthread_should_stop())
	{
		/* Lock the mutex*/
		if (mutex_lock_interruptible(&fifo_lock))
		{
			printk(KERN_ERR "Cannot get the lock\n");
			//return -1;
			return -ERESTARTSYS;
		}

		/* Wait for the condition variable */

		/* Pop the data from kfifo*/
		if(0 == kfifo_get(MY_KFIFO_NAME_P, &fifoData))
		{
			//printk(KERN_INFO "KFIFO EMPTY\n");
		}
		else	
		{
			/* Process Id and Vruntime */
			printk(KERN_INFO "Previous Process ID: %d, Vruntime: %llu\n",list_prev_entry(fifoData, tasks)->pid, list_prev_entry(fifoData, tasks)->se.vruntime);
			printk(KERN_INFO "Current Process ID: %d, Vruntime: %llu\n",fifoData->pid, fifoData->se.vruntime);
			printk(KERN_INFO "Next Process ID: %d, Vruntime: %llu\n",list_next_entry(fifoData, tasks)->pid, list_next_entry(fifoData, tasks)->se.vruntime);
			dataConsumedCount++;
		}
		
		/* Unlock the mutex*/	
		mutex_unlock(&fifo_lock);

		
	}

	printk(KERN_INFO "%s is terminated\n",__FUNCTION__);

	return dataConsumedCount;	
}

int __init gunjModule_kfifoEX_init(void)
{
	printk(KERN_INFO "Initializing kthread kfifo example Module. Function %s\n",__FUNCTION__);

	/* Init a kfifo */
	INIT_KFIFO(mykfifo);

	/* Create two threads */
	producer_task = kthread_run(producer_callback,NULL,"Producer Task");
	if(IS_ERR(producer_task))	
	{
		printk(KERN_ERR "Producer Thread run failed.\n");
		return -1;
	}

	consumer_task = kthread_run(consumer_callback,NULL,"Consumer Task");
	if(IS_ERR(consumer_task))	
	{
		int ret;
		printk(KERN_ERR "Consumer thread run failed.\n");
		ret = kthread_stop(producer_task);
		if(-1 != ret)
		{
			printk(KERN_INFO "Producer Thread has stopped with %d\n",ret);
		}
		return -1;
	}

	/* Everything went as expected */
	return 0;
}


void __exit gunjModule_kfifoEX_exit(void)
{
	/* Delete the kfifo */

	/* Stop the kthreads created */
	int ret = kthread_stop(producer_task);
	if(-1 != ret)
	{
		printk(KERN_INFO "Producer thread has stopped. Data Produced Count:%d\n",ret);
	}
	else printk(KERN_ERR "Error in Producer Thread");	

	ret = kthread_stop(consumer_task);
	if(-1 != ret)
	{
		printk(KERN_INFO "Consumer thread has stopped. Data Consumed Count:%d\n",ret);
	}
	else printk(KERN_ERR "Error in Consumer Thread");

	printk(KERN_INFO "Exiting Kthread kfifo example Module. Function %s\n",__FUNCTION__);
}


module_init(gunjModule_kfifoEX_init);
module_exit(gunjModule_kfifoEX_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gunj Manseta");
MODULE_DESCRIPTION("Module having two threads. Thread1(Producer) passes information of the currently scheduled process to the Thread2(Consumer) using a kfifo.");
MODULE_ALIAS("Gunj_Kthread_ex_Module");
