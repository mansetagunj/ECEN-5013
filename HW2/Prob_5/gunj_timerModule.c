#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

static unsigned long timer_triggered_count = 10;
static char* name = "Gunj";
static unsigned long timer_timeoutMS = 500;

module_param(name,charp,S_IRUGO | S_IWUSR);
module_param(timer_timeoutMS,ulong, S_IRUGO | S_IWUSR);

int __init gunjModule_init(void)
{
	printk(KERN_INFO "In the function %s\n",__FUNCTION__);
	printk(KERN_INFO "Param Name=%s TimeoutMS=%lu\n",name,timer_timeoutMS);
	printk(KERN_INFO "STATIC COUNT=%lu\n",timer_triggered_count++);
	return 0;
}


void __exit gunjModule_exit(void)
{

	printk(KERN_INFO "In the function %s\n",__FUNCTION__);
	printk(KERN_INFO "STATIC COUNT=%lu\n",timer_triggered_count++);
}


module_init(gunjModule_init);
module_exit(gunjModule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gunj Manseta");
MODULE_DESCRIPTION("Module having a timer of 500ms associated which logs the trivial Module author name and the times it got triggered.");
MODULE_ALIAS("Gunj_Timer_Log");
