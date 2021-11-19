/*
* Parse the OOM status information in order to activate the OOM Daemon
*/
#include "parse_oom_notifier.h"

int OOMparseFs()
{
	char buf[256];
	FILE* f;
    	int ret_val = 0;
    	snprintf(buf, sizeof(buf), "/proc/oom_notifier");
    	f = fopen(buf, "r");
	if(f == NULL)
	{
		printf("/proc/oom_notifier missing\n");
		exit(1);
	}
    	if (fscanf(f, "%d", &(ret_val)) < 1)
	{
        	printf("OOM parsing failed\n");
		exit(1);
    	}
    	fclose(f);
	return ret_val;
}
