#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/pid.h>


#define getStatusString(state) ((state > 0) ? "Stopped" : ((state == 0) ? "Runnable" : ((state == -1) ? "Unrunnable" : "Unknown")))	

#define getChildrenCount(child_taskStruct)		\
({												\
	static unsigned int child_count = 0;		\
	struct list_head *list_itr	;				\
	list_for_each(list_itr,child_taskStruct)	\
	{											\
		child_count++;							\
	}											\
	child_count;								\
})


static int process_id = -1;

module_param(process_id,int,S_IRUGO | S_IWUSR);

int __init gunjModule_proctree_init(void)
{
	struct task_struct *task;

	printk(KERN_INFO "Initializing Process tree example Module. Function %s\n",__FUNCTION__);

	if(-1 == process_id)
	{
		printk(KERN_INFO "Got no process_id as parameter. Taking current process.\n");
		task = current;	
	}
	else
	{
		struct pid *procid_struct = find_get_pid(process_id);
		task = pid_task(procid_struct, PIDTYPE_PID);
	}

	/*
	Thread Name
	Process ID
	Process Status
	Number of children
	Nice value
	*/
    printk(KERN_INFO "Process got as parameter: %s, PID: %d, State: %s, #Children: %u, Nice: %d", task->comm, task->pid, getStatusString(task->state), getChildrenCount(&task->children),  task_nice(task));
    do
    {
        task = task->parent;
        printk(KERN_INFO "Parent process: %s, PID: %d, State: %s, #Children: %u, Nice: %d", task->comm, task->pid, getStatusString(task->state), getChildrenCount(&task->children),  task_nice(task));

    }while(0 != task->pid);

	return 0;
}


void __exit gunjModule_proctree_exit(void)
{
	printk(KERN_INFO "Exiting Process tree example Module. Function %s\n",__FUNCTION__);
}


module_init(gunjModule_proctree_init);
module_exit(gunjModule_proctree_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gunj Manseta");
MODULE_DESCRIPTION("Module accepts a process id and it prints the details of all its fore-father processes traversing up the evolution of the current process lineage up until the big bang.");
MODULE_ALIAS("Gunj_processtree_Module");
