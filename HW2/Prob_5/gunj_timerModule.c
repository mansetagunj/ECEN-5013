#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/delay.h>

static char* name = "Gunj";
static unsigned long timer_timeoutMS = 500;

module_param(name,charp,S_IRUGO | S_IWUSR);
module_param(timer_timeoutMS,ulong, S_IRUGO | S_IWUSR);

struct timer_list myModuleTimer;
static unsigned long timer_triggered_count = 0;

void myTimer_callback(unsigned long data)
{
	printk(KERN_INFO "[%lu] Times triggered: %lu, Name: %s\n",jiffies,++timer_triggered_count,name);
	mod_timer(&myModuleTimer, jiffies+msecs_to_jiffies(timer_timeoutMS));
}

int __init gunjModule_init(void)
{
	printk(KERN_INFO "Initializing Gunj Timer Module. Function %s\n",__FUNCTION__);
	//printk(KERN_INFO "Param Name=%s TimeoutMS=%lu\n",name,timer_timeoutMS);
	//printk(KERN_INFO "STATIC COUNT=%lu\n",timer_triggered_count++);
	
	printk(KERN_INFO "Adding Timer with timeout:%lu\n",timer_timeoutMS);

	setup_timer(&myModuleTimer,myTimer_callback,0);
	//myModuleTimer = __TIMER_INITIALIZER(myTimer_callback, jiffies+msecs_to_jiffies(timer_timeoutMS), 0, 0);
	add_timer(&myModuleTimer);
	mod_timer(&myModuleTimer, jiffies+msecs_to_jiffies(timer_timeoutMS));

	return 0;
}


void __exit gunjModule_exit(void)
{

	int i = 0;
	/*Retrying to delete the timer gracefully till 5 attempts */	
	while (timer_pending(&myModuleTimer)) 
	{
                printk("Timer is pending\n");
                msleep(timer_timeoutMS>>3);
                i++;
                if (i == 5)
                        del_timer(&myModuleTimer);
        }
        if (i == 0) 
	{
                printk("Deleting Timer\n");	
		if(del_timer(&myModuleTimer))
			printk(KERN_INFO "Could not delete timer.Exiting module.\n");
	}

	printk(KERN_INFO "Exiting Gunj Timer Module. Function %s\n",__FUNCTION__);
}


module_init(gunjModule_init);
module_exit(gunjModule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gunj Manseta");
MODULE_DESCRIPTION("Module having a configurable timer as an input param(default 500ms) associated which logs the trivial name input param and the number of times it got triggered.");
MODULE_ALIAS("Gunj_Timer_Module");
