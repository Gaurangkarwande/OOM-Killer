#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#define NUM_PAGES 10

void handle_sigterm(int sig)
{
    printf("blocking SIGTERM %d\n", sig);
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main()
{
    FILE *fp;
    char pid_str[20];
    int priority = 10;

    long page_size = sysconf(_SC_PAGESIZE); //Inquire about the virtual memory page size of the machine
    long bs = page_size * NUM_PAGES;
    long cnt = 0, last_sum = 0;
    struct timeval tv1;
    char* p;
    int pid;
    signal(SIGTERM, handle_sigterm);
    gettimeofday(&tv1, NULL);
    pid = getpid();
    printf("Test process PID: %d\n",pid);

    sprintf(pid_str, "%d", pid);
    char* priority_file_path = concat("/tmp/user_processes/", pid_str);
    fp = fopen(priority_file_path, "w+");
    fprintf(fp, "%d", priority);
    fclose(fp);
    free(priority_file_path);
    
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