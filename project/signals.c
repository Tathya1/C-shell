#include "signals.h"


typedef struct
{
    pid_t pid;
    char name[256];
    int status; // 0: Running, 1: Stopped
} background_process;
background_process bg_processes[MAX_BG_PROCESSES];
background_process all_processes[MAX_BG_PROCESSES];
extern int all_process_count;
extern int bg_process_count;
extern int fg_pid;
extern char fg_process_name[100];
extern pid_t shell_pgid;
extern int shell_terminal;
extern int keep_running;
extern struct termios shell_tmodes;


void handle_ctrl_c(int sig)
{
    if (fg_pid > 0)
    {
        kill(fg_pid, SIGINT); 
        fg_pid = -1;          
    }
}


void handle_ctrl_d()
{
    for (int i = 0; i < all_process_count; i++)
    {
        kill(all_processes[i].pid, SIGKILL); 
    }
    printf("Logged out, all processes killed.\n");
    exit(0);
}


void handle_ctrl_z(int sig)
{
    if (fg_pid > 0)
    {
        
        if (kill(fg_pid, SIGTSTP) == 0)
        {
            add_bg_process(fg_pid, fg_process_name, 1);

            printf("Process %d (%s) moved to background and stopped.\n", fg_pid, fg_process_name);
            fg_pid = -1;
            fg_process_name[0] = '\0'; 
        }
        else
        {
            perror("Error sending SIGTSTP");
        }
    }
    else
    {
        printf("No foreground process to stop.\n");
    }
}