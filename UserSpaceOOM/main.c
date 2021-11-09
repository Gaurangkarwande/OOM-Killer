#include "get_process_statistics.h"
#include "parse_oom_notifier.h"
#include "user_space_kill.h"

int main()
{
	int Status_oom;
	while(1)
	{
		Status_oom = OOMparseFs();
		if(Status_oom == 2)
		{
			victim_kill(SIGKILL);
		}
		else if(Status_oom == 1)
		{
			victim_kill(SIGTERM);
		}
		usleep(100000);
	}

}
