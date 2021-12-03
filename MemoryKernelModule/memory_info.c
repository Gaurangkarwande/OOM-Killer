/*
* Purpose of this module is to get the memory information to activate the OOM Daemon when
* lesser memory space is available.
*/

#include "memory_info.h"
#include "Psi_Mem_interface.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[Gaurang Karwande, Nikhil Abhyankar, Radhika Nibandhe]");
MODULE_DESCRIPTION("LKP Project 3");

/*Macros*/
#define KILL_THRESHOLD_AVAILABLE_MEM 30
#define KILL_THRESHOLD_SWAP_MEM 30
#define WARN_THRESHOLD_AVAILABLE_MEM 50
#define WARN_THRESHOLD_SWAP_MEM 50



#define SYSRQ_TRIGGER_FILE  "/proc/sysrq-trigger"
#define PSI_MEMORY_FILE     "/proc/pressure/memory"
#define BUFSIZE             256


/*K-Probe Implementation*/
extern unsigned long kallsyms_lookup_name(const char* name);
#define kallsyms_lookup_name (*(typeof(&kallsyms_lookup_name))my_kallsyms_lookup_name)
void* my_kallsyms_lookup_name = NULL;

static struct kprobe kp_lookup = {
	.symbol_name = "kallsyms_lookup_name" };

extern void si_swapinfo(struct sysinfo* val);
#define si_swapinfo (*(typeof(&si_swapinfo)) kallsyms_si_swapinfo)
void* kallsyms_si_swapinfo = NULL;

/*Memory Statistics*/
static struct task_struct* mem_task;
struct meminfostat_t memory_data;


/*OOM Status Flag*/
int OOM_Status = 0;
int PSI_OOM_Kill_Status = 0;


/*Calculate Memory Statistics*/
static int calculate_memory_info(struct meminfostat_t* ms)
{
	struct sysinfo val;
	si_swapinfo(&val);
	ms->totalMemory = totalram_pages();
	ms->sys_page_size = PAGE_SIZE;
	ms->totalSwapMemory = val.totalswap;
	ms->freeSwapMemory = val.freeswap;
	ms->availableMemory = si_mem_available();
	ms->percentMemoryAvailable = (ms->availableMemory * 100) / ms->totalMemory;
	if (ms->totalSwapMemory > 0)
	{
		ms->percentFreeSwap = (ms->freeSwapMemory * 100) / ms->totalSwapMemory;
	}
	else
	{
		ms->percentFreeSwap = 0;
	}
	return 0;
}


/*Monitor Memory Info and activate the OOM killer based on the set Threshold*/
int MemoryInfoMonitor(void* data)
{
	while (!kthread_should_stop())
	{

			calculate_memory_info(&memory_data);
			printk(KERN_INFO "Memory Available in System:\t %d%% \n Memory Available for Swap:\t %d%% \n", memory_data.percentMemoryAvailable, memory_data.percentFreeSwap);
			if (((memory_data.percentMemoryAvailable <= KILL_THRESHOLD_AVAILABLE_MEM) && (memory_data.percentFreeSwap <= KILL_THRESHOLD_SWAP_MEM)) || PSI_OOM_Kill_Status == 2)
			{
				OOM_Status = 2;
			}
			else if (((memory_data.percentMemoryAvailable <= WARN_THRESHOLD_AVAILABLE_MEM) && (memory_data.percentFreeSwap <= WARN_THRESHOLD_SWAP_MEM))|| PSI_OOM_Kill_Status == 1)
			{
				OOM_Status = 1;
			}
			else
			{
				OOM_Status = 0;
			}
			msleep(500);
		}
		return 0;
}

/*Notify the User of the OOM Status to inform OOM Daemon actiation*/
static int UserSpaceNotifyInfo(struct seq_file* p_out, void* v)
{
	seq_printf(p_out, "%d\n", OOM_Status);
	return 0;
}

static int oomninfo_open(struct inode* inode, struct file* file)
{
	return single_open(file, UserSpaceNotifyInfo, NULL);
}

static const struct proc_ops oomnotifier_proc = {
  .proc_open = oomninfo_open,
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

/*static void read_file(char* filename)
{
	struct file* fp;
	fp = filp_open(filename, O_RDONLY, 0);
	printk(KERN_INFO "reading file \n");
	printk(KERN_INFO "%u", fp->f_inode->i_flags);
	printk(KERN_INFO "OOM Status: %u", fp->f_inode->i_flags);
	filp_close(fp, NULL);
}*/


/*Kallsyms Implementation*/
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
/*Invoke Memory info fucntion to get information on the memory statistics*/
static int __init mem_stat_init(void)
{
	int lookup_ret = get_access_to_kallsyms();
	if (lookup_ret < 0)
	{
		printk(KERN_INFO "Getting access to kallsyms failed\n");
		return -1;
	}
	kallsyms_si_swapinfo = (void*)kallsyms_lookup_name("si_swapinfo");
	mem_task = kthread_create(MemoryInfoMonitor, NULL, "memory_info_module_thread");
	if (mem_task)
	{
		wake_up_process(mem_task);
		printk(KERN_INFO "memory_info_module_thread started");
	}
	proc_create("oom_notifier", 0, NULL, &oomnotifier_proc);
	//read_file(PSI_MEMORY_FILE);
	//printk(KERN_INFO "OOM_Kill_Status: %d\nOOM Status: %d", OOM_Kill_Status, OOM_Status);
	printk(KERN_INFO "OOM Notifier inserted.\n");
	return 0;
}
/*Kprobe Exit*/
static void __exit mem_stat_exit(void)
{
	int ret;
	ret = kthread_stop(mem_task);
	if (!ret)
	{
		printk(KERN_INFO "memory_info_module_thread stopped");
	}
	remove_proc_entry("oom_notifier", NULL);
	printk(KERN_INFO "OOM Notifier terminated. \n");
	return;
}

module_init(mem_stat_init);
module_exit(mem_stat_exit);
