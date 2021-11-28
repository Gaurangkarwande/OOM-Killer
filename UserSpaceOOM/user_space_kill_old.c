//No longer in use. Only for reference.

/*Process for killing task in user space*/
#include "user_space_kill.h"

/*Check the numerical value of the PIDs for a process*/
static int num_chck(char* s)
{
	int num = 0;
	if (s[0] == 0)
	{
		return 0;
	}
	while (1) 
	{
        	if (s[num] == 0)
            	return 1;
        	if (isdigit(s[num]) == 0)
            	return 0;
			num++;
	}
}

/*Wait and Kill the process with the PID*/
int pollkill(int pid, int sig, char* buf)
{
	int num;
	const int poll_time = 100;
	int res_kill = kill(pid, sig);
	if(res_kill != 0)
	{
		return res_kill;
	}
	if(sig == 0)
	{
		return 0;
	}
	for(num = 0; num < poll_time; num++)
	{
		if(sig != SIGKILL)
		{
			if(OOMparseFs() == 2)
			{
				remove(buf);
				printf("Process %d priority file deleted \n", pid);
				sig = SIGKILL;
				res_kill = kill(pid,sig);
				if(res_kill != 0)
				{
					return res_kill;
				}
			}
		}
		if(!taskinfostate(pid))
		{
			printf("Process %d killed\n\n",pid);
			return 0;
		}
		usleep(poll_time * 1000);
	}
	return -1;
}

/*Killing the process based on the priority value*/
void victim_kill(int sig)
{
	int pid;
	int victim_pid = 0;
	int victim_oom_score = 0;
	int proc_oom_score;
	unsigned long victim_VmRSS = 0;
	struct processstats_t sp;
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
		if(!num_chck(dir->d_name))
			continue;
		pid = strtoul(dir->d_name, NULL,10);
		if(pid <= 1)
			continue;
		sp = FetchProcessInfo(pid);
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
	printf("Sending SIGTERM to process: %d \nOOM_score %d \nVmRSS %ld \nPriority: %d\n",victim_pid,victim_oom_score,victim_VmRSS, victim_priority);
	snprintf(buf, sizeof(buf), "/tmp/user_processes/%d", victim_pid);
	kill_ret = pollkill(victim_pid, sig, buf);
	if(kill_ret != 0)
	{
		printf("Failed to kill process %d\n",victim_pid);
	}
	return;
}