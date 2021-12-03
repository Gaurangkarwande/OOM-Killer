/*
* Purpose of this module is test the OOM Daemon based on priority.
* The priority of this test module is set to 20.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#define PAGESCOUNT 5

void sig_handle(int sig)
{
    printf("SIGTERM Blocked %d\n", sig);
}

char* join(const char *s1, const char *s2)
{
    char *store = malloc(strlen(s1) + strlen(s2) + 1); 
    strcpy(store, s1);
    strcat(store, s2);
    return store;
}

int main()
{
    FILE *file_open;
    char val_pid[20];
    int priority = 5;

    long num = 0, final_sum = 0;
    struct timeval val1;

    long page_length = sysconf(_SC_PAGESIZE); 
    long page = page_length * PAGESCOUNT;

    char* foo;
    int pid;

    signal(SIGTERM, sig_handle);
    gettimeofday(&val1, NULL);
    pid = getpid();
    printf("Test process PID: %d\n",pid);

    sprintf(val_pid, "%d", pid);
    char* priority_file_path = join("/tmp/user_processes/", val_pid);
    file_open = fopen(priority_file_path, "w+");
    fprintf(file_open, "%d", priority);
    fclose(file_open);
    free(priority_file_path);
    
    while (1) {
        foo = malloc(page);
        if (!foo) {
            printf("dynamic malloc functionality failed\n");
            continue;
        }
        for (int count = 0; count < PAGESCOUNT; count++) {
            foo[count * page_length] = 0xab;
        }
        num++;
        if (num % 1000 == 0) {
            struct timeval val2;
            long add = page * num / 1024 / 1024;

            gettimeofday(&val2, NULL);
            long delta = val2.tv_sec - val1.tv_sec;
            delta *= 1000000;
            delta = delta + val2.tv_usec - val1.tv_usec;
            long mbps = (add - final_sum) * 1000000 / delta;
            printf("|| %4ld MiB || %4ld MiB/s ||\n", add, mbps);
            final_sum = add;
            gettimeofday(&val1, NULL);
        }
    }
}