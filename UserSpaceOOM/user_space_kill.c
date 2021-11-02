#include "user_space_kill.h"

static int isnumeric(char* str)
{
	int i = 0;
	if (str[0] == 0)
	{
		return 0;
	}
	while (1) 
	{
        	if (str[i] == 0)
            	return 1;
        	if (isdigit(str[i]) == 0)
            	return 0;
        	i++;
	}
}

int wait_kill(int pid, int signal)
{
	const int wait_duration = 100;
	int i;
	int kill_res = kill(pid,signal);
	if(kill_res != 0)
	{
		return kill_res;
	}
	if(signal == 0)
	{
		return 0;
	}
	for(i = 0; i < wait_duration; i++)
	{
		if(signal != SIGKILL)
		{
			if(parseOOMNotifierFS() == 2)
			{
				signal = SIGKILL;
				kill_res = kill(pid,signal);
				if(kill_res != 0)
				{
					return kill_res;
				}
			}
		}
		if(!taskState(pid))
		{
			printf("Process %d killed\n",pid);
			return 0;
		}
		usleep(wait_duration * 1000);
	}
	return -1;
}

void kill_victim_process(int signal)
{
	int pid;
	int victim_pid = 0;
	int victim_oom_score = 0;
	int proc_oom_score;
	unsigned long victim_VmRSS = 0;
	struct statproc_t sp;
	struct dirent* dir;
	int kill_ret;
	int victim_priority = 0;
	int priority;
	char buf[256];

	DIR* user_proc_dir = opendir("/tmp/user_processes");
	if(user_proc_dir == NULL)
	{
		printf("/tmp/user_processes directory not accessible! \n");
		exit(1);
	}
	while(1)
	{
		dir = readdir(user_proc_dir);
		if(dir == NULL)
		{
			printf("/proc read error\n");
			break;
		}
		if(!isnumeric(dir->d_name))
			continue;
		pid = strtoul(dir->d_name, NULL,10);
		if(pid <= 1)
			continue;
		sp = getProcessStatistics(pid);
		priority = get_process_priority(pid);
		if (priority == 0)
			continue;
		if(sp.VmRSS == 0)
			continue;
		if(sp.exited == 1)
			continue;
		proc_oom_score = sp.oom_score;
		if(sp.oom_score_adj > 0)
			proc_oom_score -= sp.oom_score_adj;
		if(priority > victim_priority)
		{
			victim_pid = pid;
			victim_priority = priority;
			victim_oom_score = proc_oom_score;
			victim_VmRSS = sp.VmRSS;
		}
	}
	closedir(user_proc_dir);
	if(victim_pid == 0)
	{
		printf("Cannot find a victim process\n");
		return;
	}
	printf("Killing process %d with oom_score %d and VmRSS %ld and PRIORITY: %d\n",victim_pid,victim_oom_score,victim_VmRSS, victim_priority);
	snprintf(buf, sizeof(buf), "/tmp/user_processes/%d", victim_pid);
	remove(buf);
	kill_ret = wait_kill(victim_pid,signal);
	if(kill_ret != 0)
	{
		printf("Failed to kill process %d\n",victim_pid);
	}
	return;
}

