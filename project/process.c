#include "process.h"

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

void tokenize_command1(char *command, char *args[])
{
    int i = 0;
    char *token;

    
    token = strtok(command, " \t\n");
    while (token != NULL && i < max_comm_size - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

void update_process_status(pid_t pid, int status)
{
    for (int i = 0; i < all_process_count; i++)
    {
        if (all_processes[i].pid == pid)
        {
            all_processes[i].status = status;
            break;
        }
    }
}

void sigchld_handler(int sig)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        if (WIFEXITED(status))
        {
            fprintf(stderr,"Background process %d exited with status %d\n", pid, WEXITSTATUS(status));
            delete_process(pid);
        }
        else if (WIFSIGNALED(status))
        {
            fprintf(stderr,"Background process %d was terminated by signal %d\n", pid, WTERMSIG(status));
            delete_process(pid);
        }
        else if (WIFSTOPPED(status))
        {
            update_process_status(pid, 1);
            fprintf(stderr,"Process with PID %d was stopped by signal %d\n", pid, WSTOPSIG(status));
        }
    }
}
void add_bg_process(pid_t pid, char *name, int status)
{
    int f = 0;
    for (int i = 0; i < bg_process_count; i++)
    {
        if (bg_processes[i].pid == pid)
        {
            strcpy(bg_processes[i].name, name);
            bg_processes[i].status = status;
            f = 1;
        }
        if (f == 1)
        {
            break;
        }
    }
    if (f == 0)
    {
        if (bg_process_count < MAX_BG_PROCESSES)
        {
            bg_processes[bg_process_count].pid = pid;
            strcpy(bg_processes[bg_process_count].name, name);
            bg_processes[bg_process_count].status = status;
            bg_process_count++;
        }
    }

    for (int i = 0; i < all_process_count; i++)
    {
        if (all_processes[i].pid == pid)
        {
            strcpy(all_processes[i].name, name);
            all_processes[i].status = status;
            return;
        }
    }

    if (all_process_count < MAX_BG_PROCESSES)
    {
        all_processes[all_process_count].pid = pid;
        strcpy(all_processes[all_process_count].name, name);
        all_processes[all_process_count].status = status;
        all_process_count++;
    }
}

void add_fg_process(pid_t pid, char *name, int status)
{
    if (all_process_count < MAX_BG_PROCESSES)
    {
        all_processes[all_process_count].pid = pid;
        strcpy(all_processes[all_process_count].name, name);
        all_processes[all_process_count].status = status;
        all_process_count++;
    }
}

void check_bg_processes()
{
    int status;
    pid_t pid;
    for (int i = 0; i < bg_process_count; i++)
    {
        pid = waitpid(bg_processes[i].pid, &status, WNOHANG);
        if (pid > 0)
        {
            if (WIFEXITED(status))
            {
                printf("%s exited normally (%d)\n", bg_processes[i].name, pid);
                for (int j = 0; j < all_process_count; j++)
                {
                    if (all_processes[j].pid == bg_processes[i].pid)
                    {
                        all_processes[j].status = 1;
                        break;
                    }
                }
            }
            else
            {
                printf("%s exited abnormally (%d)\n", bg_processes[i].name, pid);
                for (int j = 0; j < all_process_count; j++)
                {
                    if (all_processes[j].pid == bg_processes[i].pid)
                    {
                        all_processes[j].status = 1;
                        break;
                    }
                }
            }
            bg_processes[i] = bg_processes[bg_process_count - 1];
            bg_process_count--;
            i--; 
        }
    }
}

void execute_command(char *command, char *args[], int is_background)
{
    pid_t pid = fork();
    time_t start_time, end_time;
    char duration_str[20];

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        
        setpgid(0, 0);
        execvp(args[0], args);
        printf("ERROR : '%s' is not a valid command\n", args[0]);
        exit(EXIT_FAILURE); 
    }
    else
    {
        
        if (is_background)
        {
            fprintf(stderr, "%d\n", pid);
            add_bg_process(pid, command, 0);
        }
        else
        {

            setpgid(pid, pid);              
            tcsetpgrp(shell_terminal, pid); 

            start_time = time(NULL);
            fg_pid = pid; 
            strcpy(fg_process_name, command);
            add_fg_process(fg_pid, fg_process_name, 0);
            
            int wait_status;
            waitpid(pid, &wait_status, WUNTRACED); 
            tcsetpgrp(shell_terminal, shell_pgid); 
            if (WIFSTOPPED(wait_status))
            {
                add_bg_process(fg_pid, fg_process_name, 1);
                printf("Process %d was stopped and moved to the background.\n", pid);
            }
            else
            {
                fg_pid = -1;
                end_time = time(NULL);

                long duration = end_time - start_time;
                if (duration > 2)
                {
                    if (commands_logged > 0)
                    {
                        strcat(concatenated_commands, "; ");
                    }
                    strcat(concatenated_commands, args[0]);
                    strcat(concatenated_commands, " ");
                    sprintf(duration_str, ": %ld", duration);
                    strcat(concatenated_commands, duration_str);

                    total_duration += duration;
                    commands_logged++;
                }
                delete_process(pid);
            }
        }
    }
}