#include "get_process_statistics.h"

bool taskState(int pid)
{
    char buf[256];
    snprintf(buf, sizeof(buf), "/proc/%d/stat", pid);
    FILE* f = fopen(buf, "r");
    if (f == NULL) {
        return false;
    }
    char state;
    if (fscanf(f, "%*d %*s %c", &state) < 1) {
        return false;
    }
    fclose(f);
    if (state == 'Z')
    {
        return false;
    }
    return true;
}

struct statproc_t getProcessStatistics(int pid)
{
    char buf[256];
    FILE* f;
    struct statproc_t ret_val = {0};

    snprintf(buf, sizeof(buf), "/proc/%d/oom_score", pid);
    f = fopen(buf, "r");
    if (f == NULL) {
        ret_val.exited = 1;
        return ret_val;
    }
    if (fscanf(f, "%d", &(ret_val.oom_score)) < 1){
        printf("Parsing OOM Score failed\n");
    }
    fclose(f);

    snprintf(buf, sizeof(buf), "/proc/%d/oom_score_adj", pid);
    f = fopen(buf, "r");
    if (f == NULL) {
        ret_val.exited = 1;
        return ret_val;
    }
    if (fscanf(f, "%d", &(ret_val.oom_score_adj)) < 1){
        printf("Parsing OOM Score Adj failed\n");
    }
    fclose(f);

    snprintf(buf, sizeof(buf), "/proc/%d/statm", pid);
    f = fopen(buf, "r");
    if (f == NULL) {
        ret_val.exited = 1;
        return ret_val;
    }
    if (fscanf(f, "%*u %lu", &(ret_val.VmRSS)) < 1) {
        printf("Parsing VmRSS failed\n");
    }
    ret_val.VmRSS = ret_val.VmRSS * sysconf(_SC_PAGESIZE) / 1024;
    fclose(f);

    return ret_val;
}

int get_process_priority(int pid)
{
    char buf[256];
    FILE* f;
    int priority = 0;
    snprintf(buf, sizeof(buf), "/tmp/user_processes/%d", pid);
    f = fopen(buf, "r");
    if (f == NULL) 
    {
        //printf("This seems to be a kernel process, PID: %d \n", pid);
        return priority;
    }
    if (fscanf(f, "%d", &priority) < 1)
    {
        printf("Parsing Priority failed\n");
        fclose(f);
    }
    else
    {
        printf("Process %d priority parsed \n", pid);
        fclose(f);
    }
    return priority;
}
