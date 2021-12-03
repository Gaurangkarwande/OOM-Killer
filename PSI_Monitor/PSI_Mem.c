/*
* Purpose of this module is to get the memory information to activate the OOM Daemon when
* lesser memory space is available.
*/
#include "PSI_Mem.h"
#include "../MemoryKernelModule/Psi_Mem_interface.h"

/* Daemon parameters */
#define POLL_INTERVAL       5
#define RECOVERY_INTERVAL  15
#define MEM_THRESHOLD_KILL      30
#define MEM_THRESHOLD_WARN		10

#define PSI_MEMORY_FILE     "/proc/pressure/memory"
#define BUFSIZE             256


/*OOM Status Flag*/
int PSI_OOM_Kill_Status = 0;


/*Code for PSI*/

static ssize_t fstr(const char* path, char* rbuf, const char* wbuf) {
	int fd;
	ssize_t n;

	/* one and only one operation per call */
	if ((!rbuf && !wbuf) || (rbuf && wbuf))
		return 0;

	fd = open(path, rbuf ? O_RDONLY : O_WRONLY);
	if (fd < 0)
		err(1, "%s", path);

	if (rbuf)
		n = read(fd, rbuf, BUFSIZE);
	else
		n = write(fd, wbuf, strlen(wbuf));
	if (n < 0)
		err(1, "%s", path);
	close(fd);

	if (rbuf)
		rbuf[n - 1] = '\0';

	return n;
}


int main(int argc, char** argv) {
	setvbuf(stdout, NULL, _IOLBF, 0);
	printf("poll_interval = %ds,\nrecovery_interval = %ds,\nstall_threshold = %d%%\n\n",
		POLL_INTERVAL, RECOVERY_INTERVAL, MEM_THRESHOLD_KILL);

	while (true) {
		int i;
		char buf[BUFSIZE];
		float full_avg10;

		fstr(PSI_MEMORY_FILE, buf, NULL);

		for (i = 0; buf[i] != '\n'; i++);
		i++; /* skip \n */
		i += 11; /* skip "full avg10=" */

		sscanf(buf + i, "%f", &full_avg10);
		printf("full_avg10=%f\n", full_avg10);
		if (full_avg10 > MEM_THRESHOLD_KILL)
		{
			printf("Above Threshold Limit Summoning OOM DAEMON");
			PSI_OOM_Kill_Status = 2;
			sleep(RECOVERY_INTERVAL);
		}
		else if (full_avg10 > MEM_THRESHOLD_WARN)
		{
			//printf("WARN: THhreshold Limit Approaching\n");
			PSI_OOM_Kill_Status = 1;
		}
		else
			sleep(POLL_INTERVAL);
	}

	return 0;
}


