#include "split.h"


extern int keep_running;
extern int shell_terminal;
extern pid_t shell_pgid;
struct termios shell_tmodes; // Terminal modes to restore
extern char *ali_name[max_comm_size];
extern char *ali_val[max_comm_size];
extern *fun_name[max_comm_size];
extern char **fun_body[max_comm_size];
extern int ali_c;
extern int f_c;


void init_shell()
{
    shell_terminal = STDIN_FILENO;
    shell_pgid = getpid();

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    setpgid(shell_pgid, shell_pgid);

    tcsetpgrp(shell_terminal, shell_pgid);

    tcgetattr(shell_terminal, &shell_tmodes);
}
void tokenize_command(char *command, char *args[])
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


void trim_leading_spaces(char *str)
{
    char *start = str;

    // Find the first non-space character
    while (*start == ' ' || *start == '\t')
    {
        start++;
    }

    // Shift the string to the left
    if (start != str)
    {
        char *dest = str;
        while (*start)
        {
            *dest++ = *start++;
        }
        *dest = '\0'; // Null-terminate the trimmed string
    }
}

void handle_custom_command(char *command, char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir, int is_background)
{
    for (int i = 0; i < ali_c; i++)
    {
        if (strncmp(command, ali_name[i], strlen(ali_name[i])) == 0)
        {
            char remaining[MAX_SIZE] = {0};

            if (strlen(command) > strlen(ali_name[i]))
            {
                strcpy(remaining, command + strlen(ali_name[i]));
            }

            strcpy(command, ali_val[i]);

            strcat(command, remaining);

            break; // Exit the loop once alias is found and replaced
        }
    }

    trim_whitespace(command);
    char temp[MAX_SIZE];
    strcpy(temp, command);

    tokenize_command(command, args);
    int flag = 0;
    for (int i = 0; i < f_c; i++)
    {

        if (strncmp(command, fun_name[i], strlen(fun_name[i])) == 0)
        {
            for (int j = 0; fun_body[i][j] != NULL; j++)
            {
                // printf("%s",fun_body[i][j]);
                trim_whitespace(fun_body[i][j]);
                trim_leading_spaces(fun_body[i][j]);

                for (int k = 0; k < strlen(fun_body[i][j]); k++)
                {
                    if (fun_body[i][j][k] == '"')
                    {
                        fun_body[i][j][k - 1] = '\0';
                        break;
                    }
                }
                if (args[1] != NULL)
                {
                    strcat(fun_body[i][j], " ");
                    strcat(fun_body[i][j], args[1]);
                }

                // printf("%s\n", fun_body[i][j]);

                // Create a buffer for the command
                char *args1[max_comm_size];
                char comm12[MAX_SIZE]; // Allocate a local buffer
                strcpy(comm12, fun_body[i][j]);

                // Tokenize and execute the command
                char comm13[MAX_SIZE]; // Allocate a local buffer
                strcpy(comm13,comm12);

                tokenize_command(comm12, args1);
                handle_custom_command(comm13, args1, current_dir, home_dir, prev_dir, relative_dir, is_background);
            }
            flag = 1;
            break;
        }
    }
    if (flag == 1)
        return 1;


    if (strncmp(command, "hop", 3) == 0)
    {
        hop(args, current_dir, home_dir, prev_dir, relative_dir);
    }
    else if (strncmp(command, "reveal", 6) == 0)
    {
        process_command(args, home_dir, prev_dir);
    }
    else if (strncmp(command, "log", 3) == 0)
    {
        do_log(args, current_dir, home_dir, prev_dir, relative_dir);
    }
    else if (strncmp(command, "proclore", 8) == 0)
    {
        handle_proclore(args);
    }
    else if (strncmp(command, "seek", 4) == 0)
    {
        seek_command(args, home_dir, prev_dir);
    }
    else if (strcmp(command, "activities") == 0)
    {
        activities();
    }
    else if (strncmp(command, "ping", 4) == 0)
    {
        if (args[1] == NULL || args[2] == NULL)
        {
            printf("Error: Missing arguments. Usage: ping <pid> <signal_number>\n");
            return 1; // Return error code
        }
        int pid = atoi(args[1]);
        int signum = atoi(args[2]);
        ping(pid, signum);
    }
    else if (strncmp(command, "neonate", 8) == 0)
    {
        if (args[2] == NULL)
        {
            printf("Error: Missing arguments\n");
            return 1; // Return error code
        }
        neonate(atoi(args[2]));
    }
    else if (strncmp(command, "fg", 2) == 0)
    {
        if (args[1] == NULL)
        {
            printf("Error: Missing arguments\n");
            return 1; // Return error code
        }

        push_fore(atoi(args[1]));
    }
    else if (strncmp(command, "bg", 2) == 0)
    {
        if (args[1] == NULL)
        {
            printf("Error: Missing arguments\n");
            return 1; // Return error code
        }
        printf("%d\n", atoi(args[1]));

        push_back(atoi(args[1]));
    }

    else if (strncmp(command, "iMan", 4) == 0)
    {
        fetch_man_page(temp);
    }

    else
    {
        check_bg_processes();
        execute_command(temp, args, is_background);
    }
}
