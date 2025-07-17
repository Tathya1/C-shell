#include "act.h"

typedef struct
{
    pid_t pid;
    char name[256];
    int status; // 0: Running, 1: Stopped
} background_process;
extern background_process bg_processes[MAX_BG_PROCESSES];
extern background_process all_processes[MAX_BG_PROCESSES];
extern int all_process_count;
extern int bg_process_count;

int compare_processes(const void *a, const void *b)
{
    background_process *process_a = (background_process *)a;
    background_process *process_b = (background_process *)b;
    return strcmp(process_a->name, process_b->name);
}

void activities()
{

    qsort(all_processes, all_process_count, sizeof(background_process), compare_processes);

    for (int i = 0; i < all_process_count; i++)
    {
        char *status_str = (all_processes[i].status == 0) ? "Running" : (all_processes[i].status == 1) ? "Stopped"
                                                                                                       : "Exited";
        printf("%d : %s - %s\n", all_processes[i].pid, all_processes[i].name, status_str);
    }
}