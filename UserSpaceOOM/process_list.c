#include "process_list.h"

void init_process_list( Process_List *a, size_t initialSize) 
{
  a->array = malloc(initialSize * sizeof( Process));
  a->used = 0;
  a->size = initialSize;
}

void insert_process( Process_List *a, int pid, int weight, int value, unsigned long VmRSS) 
{
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  Process my_process;
  my_process.pid = pid;
  my_process.weight = weight;
  my_process.VmRSS = VmRSS;
  my_process.value = value;

  if (a->used == a->size) {
    a->size += 1;
    a->array = realloc(a->array, a->size * sizeof(Process));
  }
  a->array[a->used++] = my_process;
}

void free_process_list(Process_List *a) 
{
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

void print_process_list(Process_List *a)
{
  //int num_processes = (int) a->size;
  printf("Printing processes\n");
  for (size_t i = 0; i < a->used; i++)
  {
    Process my_process = a->array[i];
    printf("Process: %d Weight %d Value: %d VmRSS: %ld \n",my_process.pid,my_process.weight, my_process.value, my_process.VmRSS);
  }
  return;
}

/*
int main()
{
    Process_List my_process_list;
    init_process_list(&my_process_list, 2);
    insert_process(&my_process_list, 0, 100, 50, 69);
    insert_process(&my_process_list, 0, 300, 5, 69);
    insert_process(&my_process_list, 0, 50, 50, 69);
    insert_process(&my_process_list, 0, 150, 90, 69);
    insert_process(&my_process_list, 0, 20, 23, 69);
    print_process_list(&my_process_list);

    free_process_list(&my_process_list);
    return 0;

}
*/
