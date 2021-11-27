#include "knapsack.h"

int *knapsack (Process *items, int n, int w) {
    int i, j, a, b, *mm, **m, *s;
    mm = calloc((n + 1) * (w + 1), sizeof (int));
    m = malloc((n + 1) * sizeof (int *));
    m[0] = mm;
    for (i = 1; i <= n; i++) {
        m[i] = &mm[i * (w + 1)];
        for (j = 0; j <= w; j++) {
            if (items[i - 1].weight > j) {
                m[i][j] = m[i - 1][j];
            }
            else {
                a = m[i - 1][j];
                b = m[i - 1][j - items[i - 1].weight] + items[i - 1].value;
                m[i][j] = a > b ? a : b;
            }
        }
    }
    s = calloc(n, sizeof (int));
    for (i = n, j = w; i > 0; i--) {
        if (m[i][j] > m[i - 1][j]) {
            s[i - 1] = 1;
            j -= items[i - 1].weight;
        }
    }
    free(mm);
    free(m);
    return s;
}

/*

int main()
{
    int i, n, victim_pid, victim_weight, victim_value, tw = 0, tv = 0, *s;
    unsigned long victim_VmRSS;
    Process_List my_process_list;
    init_process_list(&my_process_list, 2);
    insert_process(&my_process_list, 0, 100, 50, 69);
    insert_process(&my_process_list, 1, 300, 5, 69);
    insert_process(&my_process_list, 2, 50, 50, 69);
    insert_process(&my_process_list, 3, 150, 90, 69);
    insert_process(&my_process_list, 4, 20, 23, 69);
    insert_process(&my_process_list, 5, 100, 5, 69);
    insert_process(&my_process_list, 6, 250, 5, 69);
    insert_process(&my_process_list, 7, 75, 5, 69);
    print_process_list(&my_process_list);
    printf("\n\nSending to knapsack \n\n");
    n = (int) my_process_list.used;
    s = knapsack(my_process_list.array, n, 400);

    for (i = 0; i < n; i++) {
        if (s[i] == 0) {
            victim_pid = my_process_list.array[i].pid;
            victim_weight = my_process_list.array[i].weight;
            victim_value = my_process_list.array[i].value;
            victim_VmRSS = my_process_list.array[i].VmRSS;
            printf("Sending SIGTERM to Process: %d Weight %d Value: %d VmRSS: %ld \n",victim_pid, victim_weight, victim_value, victim_VmRSS);
            tw += victim_weight;
            tv += victim_value;
        }
    }
    printf("Freed %-22s %5d %5d\n", "totals:", tw, tv);

    free_process_list(&my_process_list);
    return 0;

}

*/
