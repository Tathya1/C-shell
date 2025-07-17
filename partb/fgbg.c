#include "fgbg.h"


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
extern int fg_pid;
extern char fg_process_name[100];
extern pid_t shell_pgid;
extern int shell_terminal;
extern int keep_running;
extern struct termios shell_tmodes;

void update_process_state(int pid) {
    char path[256];
    char buffer[1024];
    char comm[256];
    char state;
    FILE *fp;

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening stat file");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "%d %s %c", &pid, comm, &state);

        for (int i = 0; i < all_process_count; i++) {
            if (all_processes[i].pid == pid) {
                if (state == 'S' || state == 'R') {
                    all_processes[i].status=0;
                } else if (state == 'T') {
                    all_processes[i].status=1;
                } else if (state == 'Z') {
                    all_processes[i].status=2;
                } else {
                    all_processes[i].status=2;
                }

                printf("Process %d is now in state: %s\n", pid, all_processes[i].status);
                break;
            }
        }
    }

    fclose(fp);
}



void ping(pid_t pid, int signal_num)
{
    signal_num = signal_num % 32;
    if (kill(pid, 0) == -1)
    {
        if (errno == ESRCH)
        {
            printf("No such process found\n");
        }
        else
        {
            perror("Error checking process");
        }
        return;
    }



    if (kill(pid, signal_num) == 0)
    {
        printf("Sent signal %d to process with pid %d\n", signal_num, pid);


        for (int i = 0; i < all_process_count; i++)
        {
            if (all_processes[i].pid == pid)
            {
                if (signal_num == SIGKILL || signal_num == SIGTERM)
                {
                    for (int j = i; j < all_process_count - 1; j++)
                    {
                        all_processes[j] = all_processes[j + 1];
                    }
                    all_process_count--;
                    printf("Process %d removed from the list\n", pid);
                }
                
                break;
            }
        }
    }
    else
    {
        perror("Error sending signal");
    }
}

void delete_process(pid_t pid)
{
    for (int i = 0; i < all_process_count; i++)
    {
        if (all_processes[i].pid == pid)
        {
            for (int j = i; j < all_process_count - 1; j++)
            {
                all_processes[j] = all_processes[j + 1];
            }
            all_process_count--;
        }
    }
}


void push_fore(pid_t pid)
{
    char stat_path[256];
    char stat_content[1024];
    char state;
    int fd;

    if (kill(pid, 0) == -1)
    {
        perror("No such process found");
        return;
    }

    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
    fd = open(stat_path, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening stat file");
        return;
    }

    if (read(fd, stat_content, sizeof(stat_content) - 1) <= 0)
    {
        perror("Error reading stat file");
        close(fd);
        return;
    }
    close(fd);

    sscanf(stat_content, "%*d %*s %c", &state);

    if (state != 'T' && state != 'S' && state != 'R')
    {
        printf("Process %d is not in a valid state to be brought to foreground\n", pid);
        return;
    }

    pid_t pgid = getpgid(pid);
    fg_pid = pid;
    if (pgid == -1)
    {
        perror("Error getting process group");
        return;
    }

    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    
    if (kill(-pgid, SIGCONT) == -1)
    {
        perror("Error sending SIGCONT");
        tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
        return;
    }

    int status;
    pid_t result;

    do
    {
        result = waitpid(pid, &status, WUNTRACED);
    } while (result == -1 && errno == EINTR);

    if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
    {
        perror("Error bringing shell back to foreground");
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    if (result == -1)
    {
        perror("Error waiting for process");
    }
    else if (WIFSTOPPED(status))
    {
        printf("\nProcess %d stopped\n", pid);
    }
    else if (WIFEXITED(status))
    {
        printf("\nProcess %d exited with status %d\n", pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("\nProcess %d killed by signal %d\n", pid, WTERMSIG(status));
    }
}
void push_back(int pid)
{
    int found = 0;

    for (int i = 0; i < all_process_count; i++)
    {
        if (all_processes[i].pid == pid)
        {
            found = 1;

            if (all_processes[i].status == 1) 
            {
                if (kill(pid, SIGCONT) < 0)
                {
                    perror("Error resuming process");
                    return;
                }

                all_processes[i].status = 0; 

                printf("Process [%d] is now running in the background\n", pid);
            }
            else
            {
                printf("Process [%d] is already running in the background\n", pid);
            }
            break;
        }
    }

    if (!found)
    {
        printf("No such process found\n");
    }
} 
