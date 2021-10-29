#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_PAGES 10

void handle_sigterm(int sig)
{
    printf("blocking SIGTERM %d\n", sig);
}

int main()
{
    long page_size = sysconf(_SC_PAGESIZE);
    long bs = page_size * NUM_PAGES;
    long cnt = 0, last_sum = 0;
    struct timeval tv1;
    char* p;
    int pid;
    signal(SIGTERM, handle_sigterm);
    gettimeofday(&tv1, NULL);
    pid = getpid();
    printf("Test process PID: %d\n",pid);
    while (1) {
        p = malloc(bs);
        if (!p) {
            printf("malloc failed\n");
            continue;
        }
        for (int i = 0; i < NUM_PAGES; i++) {
            p[i * page_size] = 0xab;
        }
        cnt++;
        if (cnt % 1000 == 0) {
            long sum = bs * cnt / 1024 / 1024;
            struct timeval tv2;
            gettimeofday(&tv2, NULL);
            long delta = tv2.tv_sec - tv1.tv_sec;
            delta *= 1000000;
            delta = delta + tv2.tv_usec - tv1.tv_usec;
            long mbps = (sum - last_sum) * 1000000 / delta;
            printf("%4ld MiB (%4ld MiB/s)\n", sum, mbps);
            last_sum = sum;
            gettimeofday(&tv1, NULL);
        }
    }
}