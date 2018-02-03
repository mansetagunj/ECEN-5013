#include <linux/init.h>
#include <linux/module.h>
#include <moduleparam.h>

static unsigned long timer_triggered_count = 0;
static char* name = "Gunj"
static unsigned long timer_timeoutMS = 500;

module_param(name,charp,S_IRUGO | S_IWUSR);
module_param(timer_timeoutMS,ulong, S_IRUGO | S_IWUSR);
module_param(timer_triggered_count,ulong, S_IRUGO);

int __init module_init()
{

}


int __exit module_exit()
{


}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gunj Manseta");
MODULE_DESCRIPTION("Module having a timer of 500ms associated which logs the trivial Module author name and the times it got triggered.");
MODULE_ALIAS("Gunj_Timer_Log");
