#include "split.h"
#include "man.h"
#include "process.h"
#include "signals.h"
#include "io.h"
#include "fgbg.h"
#include "act.h"
#include "log.h"
#include "hop.h"
#include "reveal.h"
#include "proclore.h"
#include "neonate.h"



char concatenated_commands[256 * 100];
long total_duration = 0;
int commands_logged = 0;
char log_path[MAX_SIZE];
int bg_process_count = 0;
int all_process_count = 0;
int fg_pid = -1;
char fg_process_name[100];
pid_t shell_pgid; // Process group ID for the shell
int shell_terminal;
int keep_running = 1;
char *ali_name[max_comm_size];
char *ali_val[max_comm_size];
char *fun_name[max_comm_size];
char** fun_body[max_comm_size];
int ali_c = 0;
int f_c = 0;

void load_myshrc(const char *filepath)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        printf("Error: Unable to open .myshrc file.\n");
        return;
    }

    char line[MAX_SIZE];

    while (fgets(line, sizeof(line), file))
    {
        if (strchr(line, '='))
        {
            char *alias_name = strtok(line, "=");
            char *alias_value = strtok(NULL, "\n");

            printf("Alias: %s\nValue: %s\n", alias_name, alias_value);

            ali_name[ali_c] = (char *)malloc(strlen(alias_name) + 1);
            ali_val[ali_c] = (char *)malloc(strlen(alias_value) + 1);

            strcpy(ali_name[ali_c], alias_name);
            strcpy(ali_val[ali_c], alias_value);
            ali_c++;
        }

        else if (strstr(line, "()"))
        {
            char *func_name = strtok(line, "()");

            fun_name[f_c] = (char *)malloc(strlen(func_name) + 1);
            strcpy(fun_name[f_c], func_name);

            printf("Function: %s\n", func_name);

            // Allocate memory for function body lines
            fun_body[f_c] = (char **)malloc(sizeof(char *) * max_comm_size);
            int line_index = 0;  // To keep track of the number of lines in the function body

            // Read function body
            while (fgets(line, sizeof(line), file))
            {
                if (strchr(line, '{'))
                {
                    continue; // Skip the opening brace
                }
                if (strchr(line, '}'))
                {
                    break; // Stop when we reach the closing brace
                }

                // Allocate memory for each line of the function body and store it
                fun_body[f_c][line_index] = (char *)malloc(strlen(line) + 1);
                strcpy(fun_body[f_c][line_index], line);
                line_index++;
            }

            // Mark the end of the function body with NULL
            fun_body[f_c][line_index] = NULL;

            printf("Function Body for %s:\n", fun_name[f_c]);
            for (int i = 0; i < line_index; i++)
            {
                printf("%s", fun_body[f_c][i]);  // Print each line of the function body
            }
            
            f_c++;  // Move to the next function
        }
    }

    fclose(file);
}

int main()

{
    init_shell();
    load_myshrc("partb/.myshrc");
    struct passwd *name = getpwuid(getuid());
    char username[MAX_SIZE], system_name[MAX_SIZE], current_dir[MAX_SIZE], relative_dir[MAX_SIZE], home_dir[MAX_SIZE], prev_dir[MAX_SIZE], input[MAX_SIZE];
    char commands[max_comm_size][max_comm_size][max_comm_size];
    int rows, cols[max_comm_size];
    // int bg_flags[max_comm_size][max_comm_size];
    int **bg_flags = (int **)malloc(max_comm_size * sizeof(int *));
    signal(SIGCHLD, sigchld_handler);
    signal(SIGTSTP, handle_ctrl_z);
    signal(SIGINT, handle_ctrl_c);
    signal(SIGKILL, handle_ctrl_d);
    if (bg_flags == NULL)
    {
        perror("Failed to allocate memory for bg_flags");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < max_comm_size; i++)
    {
        bg_flags[i] = (int *)malloc(max_comm_size * sizeof(int));

        if (bg_flags[i] == NULL)
        {
            perror("Failed to allocate memory for bg_flags row");
            exit(EXIT_FAILURE);
        }
    }
    char *args[max_comm_size];

    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");

        return 1;
    }
    strcpy(log_path, home_dir);
    strcat(log_path, "/log.txt");

    // printf("Log file path: %s\n", log_path);

    FILE *file = fopen(log_path, "a+");
    if (file == NULL)
    {
        perror("Error creating or opening the log file");
        return 1;
    }

    // fprintf(file, "Log file initialized.\n");

    fclose(file);

    if (name == NULL)
    {
        perror("getpwuid");
        return 1;
    }
    else
    {
        strcpy(username, name->pw_name);
    }
    struct utsname sys;

    if (uname(&sys) == 0)
    {
        strcpy(system_name, sys.nodename);
    }
    else
    {
        perror("uname");
        return 1;
    }
    while (1)
    {

        if (getcwd(current_dir, sizeof(current_dir)) == NULL)
        {
            perror("getcwd");
            return 1;
        }

        rel_dir(relative_dir, current_dir, home_dir);
        if (commands_logged > 0)
        {
            printf("<%s@%s:%s %s  %lds> ", username, system_name, relative_dir, concatenated_commands, total_duration);
        }
        else
            printf("<%s@%s:%s> ", username, system_name, relative_dir);
        check_bg_processes();
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            if (feof(stdin))
            {
                handle_ctrl_d();
            }
        }


        check_bg_processes();

        if (strcmp(input, "exit\n") == 0)
            break;
        if (strcmp(input, "\n") == 0)
            continue;
        concatenated_commands[0] = '\0';
        total_duration = 0;
        commands_logged = 0;
        store_in_log(input);
        sep_input(input, commands, &rows, cols, bg_flags, args, home_dir, prev_dir, relative_dir, current_dir);

        

        check_bg_processes();
    }

    for (int i = 0; i < max_comm_size; i++)
    {
        free(bg_flags[i]);
    }
    free(bg_flags);

    return 0;
}