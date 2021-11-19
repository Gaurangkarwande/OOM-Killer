/*
* The purpose of this module is to get the process statistics such as the oom score and priority.
*/

#ifndef GET_PROCESS_STATISTICS_H
#define GET_PROCESS_STATISTICS_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>
#include <stdbool.h>

struct processstats_t {
    int oom_score;
    int oom_score_adj;
    unsigned long VmRSS;
    int exited;
};

bool taskinfostate(int pid);
struct processstats_t FetchProcessInfo(int pid);
int get_process_priority(int pid);

#endif
