#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int weight;
    int value;
    unsigned long VmRSS;
} Process;

typedef struct {
  Process *array;
  size_t used;
  size_t size;
} Process_List;

void init_process_list( Process_List *a, size_t initialSize);
void insert_process( Process_List *a, int pid, int weight, int value, unsigned long VmRSS);
void free_process_list( Process_List *a);
void print_process_list(Process_List *a);

#endif