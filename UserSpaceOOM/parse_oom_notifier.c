/*---------------------------
|   AUTHOR: Gaurang Karwande
|
*----------------------------*/

#include "parse_oom_notifier.h"

int parseOOMNotifierFS()
{
	char buf[256];
	FILE* f;
    	int ret_val = 0;
    	snprintf(buf, sizeof(buf), "/proc/oom_notifier");
    	f = fopen(buf, "r");
	if(f == NULL)
	{
		printf("/proc/oom_notifier not found\n");
		exit(1);
	}
    	if (fscanf(f, "%d", &(ret_val)) < 1)
	{
        	printf("Parsing OOM Notifier Failed\n");
		exit(1);
    	}
    	fclose(f);
	return ret_val;
}
