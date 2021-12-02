/*
* Purpose of this module is to get the memory information to activate the OOM Daemon when
* lesser memory space is available.
*/

#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H


#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/swap.h>
#include <linux/mm.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kprobes.h>
#include <asm/uaccess.h>


/*structure to access Memory Information*/
struct meminfostat_t
{
	int sys_page_size;
	long totalMemory;
	long availableMemory;
	int totalSwapMemory;
	int freeSwapMemory;
	int percentMemoryAvailable;
	int percentFreeSwap;
};

#endif
