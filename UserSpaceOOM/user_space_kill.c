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
void get_processes(Process_List *my_process_list)
{
	int pid, proc_oom_score, priority;
	struct processstats_t sp;
	struct dirent* dir;

	DIR* user_proc_dir = opendir("/tmp/user_processes");
	if(user_proc_dir == NULL)
	{
		printf("/tmp/user_processes directory not accessible! \n");
		exit(1);
	}
	init_process_list(my_process_list, 2);

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
		insert_process(my_process_list, pid, proc_oom_score, priority, sp.VmRSS);
	}
	closedir(user_proc_dir);
	return;
}

void victim_kill(int sig) {
    int i, n, kill_ret, victim_pid, victim_oom_score, victim_value, tw = 0, tv = 0, *s;
	unsigned long victim_VmRSS;
	Process_List my_process_list;
    char buf[256];
    get_processes(&my_process_list);
    n = (int) my_process_list.used;
    s = knapsack(my_process_list.array, n, 1000);
    for (i = 0; i < n; i++) {
        if (s[i] == 0) {
            victim_pid = my_process_list.array[i].pid;
            victim_oom_score = my_process_list.array[i].weight;
            victim_value = my_process_list.array[i].value;
            victim_VmRSS = my_process_list.array[i].VmRSS;
            printf("Sending SIGTERM to Process: %d OOM_score %d Value: %d VmRSS: %ld \n",victim_pid,victim_oom_score, victim_value, victim_VmRSS);
            snprintf(buf, sizeof(buf), "/tmp/user_processes/%d", victim_pid);
            kill_ret = pollkill(victim_pid, sig, buf);
			if(kill_ret != 0)
			{
				printf("Failed to kill process %d\n",victim_pid);
			}
            tw += victim_oom_score;
            tv += victim_value;
        }
    }
    printf("Freed %-22s %5d %5d\n", "totals:", tw, tv);
    free_process_list(&my_process_list);
}



