#include "memory_info.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[Gaurang Karwande]");
MODULE_DESCRIPTION("LKP Project 3");

#define AVAILABLE_MEMORY_KILL_THRESHOLD 5
#define SWAP_AVAILABLE_KILL_THRESHOLD 5
#define AVAILABLE_MEMORY_WARN_THRESHOLD 10
#define SWAP_AVAILABLE_WARN_THRESHOLD 10

extern unsigned long kallsyms_lookup_name(const char *name);
#define kallsyms_lookup_name (*(typeof(&kallsyms_lookup_name))my_kallsyms_lookup_name)
void *my_kallsyms_lookup_name = NULL;

static struct kprobe kp_lookup = {
	.symbol_name = "kallsyms_lookup_name"};

extern void si_swapinfo(struct sysinfo *val);
#define si_swapinfo (*(typeof(&si_swapinfo)) kallsyms_si_swapinfo)
void *kallsyms_si_swapinfo = NULL;

static struct task_struct* memory_info_task;
struct statmem_t memstat;

int isOOMSituation = 0;

static int get_memory_statistics(struct statmem_t* ms)
{
	struct sysinfo val;
	si_swapinfo(&val); 
	ms->totalMemory = totalram_pages();
	ms->sys_page_size = PAGE_SIZE;
	ms->totalSwapMemory = val.totalswap;
	ms->freeSwapMemory = val.freeswap;
	ms->availableMemory = si_mem_available();
	ms->percentMemoryAvailable = (ms->availableMemory * 100)/ms->totalMemory;
	if(ms->totalSwapMemory > 0)
	{
		ms->percentFreeSwap = (ms->freeSwapMemory * 100)/ ms->totalSwapMemory;
	}
	else
	{
		ms->percentFreeSwap = 0;
	}
	return 0;
}

int monitorMemoryStatistics(void *data)
{
	while(!kthread_should_stop())
	{
		get_memory_statistics(&memstat);
		printk(KERN_INFO "Available Memory: %d%%, Available Swap: %d%% \n",memstat.percentMemoryAvailable,memstat.percentFreeSwap);
		if((memstat.percentMemoryAvailable <= AVAILABLE_MEMORY_KILL_THRESHOLD) && (memstat.percentFreeSwap <= SWAP_AVAILABLE_KILL_THRESHOLD))
		{
			isOOMSituation = 2;
		}
		else if((memstat.percentMemoryAvailable <= AVAILABLE_MEMORY_WARN_THRESHOLD) && (memstat.percentFreeSwap <= SWAP_AVAILABLE_WARN_THRESHOLD))
		{
			isOOMSituation = 1;
		}
		else
		{
			isOOMSituation = 0;
		}
		msleep(500);
	}
	return 0;
}

static int notifyUserSpace(struct seq_file* p_out, void* v)
{
	seq_printf(p_out,"%d\n",isOOMSituation);
	return 0;
}

static int oomnotifier_open(struct inode* inode, struct file* file)
{
	return single_open(file,notifyUserSpace,NULL);
}

static const struct proc_ops oomnotifier_fops = {
  .proc_open = oomnotifier_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};
/*
static const struct file_operations oomnotifier_fops = {
	.owner = THIS_MODULE,
	.open = oomnotifier_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};
*/

static int get_access_to_kallsyms(void)
{
	int lookup_ret;
	lookup_ret = register_kprobe(&kp_lookup);
	if (lookup_ret < 0)
	{
		printk(KERN_INFO "probing kallsyms_lookup_name failed, returned %d\n", lookup_ret);
		return -1;
	}
	my_kallsyms_lookup_name = kp_lookup.addr;
	unregister_kprobe(&kp_lookup);
	return 0;
}

static int __init stat_init(void)
{
	int lookup_ret = get_access_to_kallsyms();
	if (lookup_ret < 0)
	{
		printk(KERN_INFO "Getting access to kallsyms failed\n");
		return -1;
	}
	kallsyms_si_swapinfo = (void*)kallsyms_lookup_name("si_swapinfo");
	memory_info_task = kthread_create(monitorMemoryStatistics,NULL,"memory_info_module_thread");
	if(memory_info_task)
	{
		wake_up_process(memory_info_task);
		printk(KERN_INFO "memory_info_module_thread started");
	}
	proc_create("oom_notifier",0,NULL,&oomnotifier_fops);
	printk(KERN_INFO "OOM Notifier Module inserted.\n");
	return 0;
}

static void __exit stat_exit(void)
{
	int ret;
	ret = kthread_stop(memory_info_task);
	if(!ret)
	{
		printk(KERN_INFO "memory_info_module_thread stopped");
	}
	remove_proc_entry("oom_notifier",NULL);
	printk(KERN_INFO "OOM Notifier stopped. \n");
	return;
}

module_init(stat_init);
module_exit(stat_exit);
