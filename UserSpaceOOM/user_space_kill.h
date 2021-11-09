/*Process for killing task in user space*/
#ifndef USER_SPACE_KILL_H
#define USER_SPACE_KILL_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include "get_process_statistics.h"
#include "parse_oom_notifier.h"

void victim_kill(int sig);

#endif

