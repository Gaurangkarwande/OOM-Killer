/*---------------------------
|   AUTHOR: Gaurang Karwande
|
*----------------------------*/

#include "get_process_statistics.h"
#include "parse_oom_notifier.h"
#include "user_space_kill.h"

int main()
{
	int oomCondition;
	while(1)
	{
		oomCondition = parseOOMNotifierFS();
		if(oomCondition == 2)
		{
			kill_victim_process(SIGKILL);
		}
		else if(oomCondition == 1)
		{
			kill_victim_process(SIGTERM);
		}
		usleep(100000);
	}

}
