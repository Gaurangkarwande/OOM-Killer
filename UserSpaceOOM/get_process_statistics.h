/*---------------------------
|   AUTHOR: Gaurang Karwande
|
*----------------------------*/

#ifndef GET_PROCESS_STATISTICS_H
#define GET_PROCESS_STATISTICS_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>
#include <stdbool.h>

struct statproc_t {
    int oom_score;
    int oom_score_adj;
    unsigned long VmRSS;
    int exited;
};

bool taskState(int pid);
struct statproc_t getProcessStatistics(int pid);

#endif
