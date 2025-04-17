#include "process.h"


typedef struct
{
    pid_t pid;
    char name[256];
}
background_process;
background_process bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;

void sigchld_handler(int sig)
{
    int status;
    pid_t pid;

    // Loop to handle all terminated child processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Background process %d exited with status %d\n", pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Background process %d was terminated by signal %d\n", pid, WTERMSIG(status));
        }
    }
}
void add_bg_process(pid_t pid, char *name)
{
    if (bg_process_count < MAX_BG_PROCESSES)
    {
        bg_processes[bg_process_count].pid = pid;
        strcpy(bg_processes[bg_process_count].name, name);
        bg_process_count++;
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
            }
            else
            {
                printf("%s exited abnormally (%d)\n", bg_processes[i].name, pid);
            }
            // Remove the process from the list
            bg_processes[i] = bg_processes[bg_process_count - 1];
            bg_process_count--;
            i--; // Check the current index again as it was overwritten
        }
    }
}


void execute_command(char *args[], int is_background)
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
        // Child process
        if (is_background)
        {
            setsid(); // Separate process group for background process
        }
        execvp(args[0], args);
        printf("ERROR : '%s' is not a valid command\n", args[0]);
        exit(EXIT_FAILURE); // If execvp fails, exit child process
    }
    else
    {
        // Parent process
        if (is_background)
        {
            printf("%d\n", pid); // Print background process PID
            add_bg_process(pid, args[0]);
        }
        else
        {
            start_time = time(NULL);
            waitpid(pid, NULL, 0); // Wait for the foreground process
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
                sprintf(duration_str, " : %ld", duration);
                strcat(concatenated_commands, duration_str);
                // strcat(concatenated_commands, " ");
                total_duration += duration;

                // Add to total duration

                // Increment the number of logged commands
                commands_logged++;
            }
        }
    }
}

