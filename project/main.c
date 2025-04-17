#include "split.h"

char concatenated_commands[256 * 100];
long total_duration = 0;
int commands_logged = 0;
char log_path[MAX_SIZE];
int bg_process_count=0;




int main()
{
    struct passwd *name = getpwuid(getuid());
    char username[MAX_SIZE], system_name[MAX_SIZE], current_dir[MAX_SIZE], relative_dir[MAX_SIZE], home_dir[MAX_SIZE], prev_dir[MAX_SIZE], input[MAX_SIZE];
    char commands[max_comm_size][max_comm_size][max_comm_size];
    int rows, cols[max_comm_size];
    // int bg_flags[max_comm_size][max_comm_size];
    int **bg_flags = (int **)malloc(max_comm_size * sizeof(int *));

    // Check if the memory allocation was successful
    if (bg_flags == NULL)
    {
        perror("Failed to allocate memory for bg_flags");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each row
    for (int i = 0; i < max_comm_size; i++)
    {
        bg_flags[i] = (int *)malloc(max_comm_size * sizeof(int));

        // Check if the memory allocation for each row was successful
        if (bg_flags[i] == NULL)
        {
            perror("Failed to allocate memory for bg_flags row");
            exit(EXIT_FAILURE);
        }
    }
    char *args[max_comm_size];

    struct sigaction sa;


    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

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
        if (commands_logged>0)
        {
            printf("<%s@%s:%s %s  %lds> ", username, system_name, relative_dir, concatenated_commands, total_duration);


        }
        else
            printf("<%s@%s:%s> ", username, system_name, relative_dir);
        check_bg_processes();
        fgets(input, sizeof(input), stdin);

        check_bg_processes();

        if (strcmp(input,"exit\n")==0)
            break;
        if (strcmp(input, "\n") == 0)
            continue;
        concatenated_commands[0] = '\0';
        total_duration = 0;
        commands_logged = 0;
        store_in_log(input);
        sep_input(input, commands, &rows, cols, bg_flags);
        // printf("%d",rows);

        for (int i = 0; i < rows; i++)
        {
            // printf("Command Group %d:\n", i + 1);
            for (int j = 0; j < cols[i]; j++)
            {
                // printf("%s %d\n", commands[i][j], bg_flags[i][j]);

                // printf("%s\n",relative_dir);
                handle_custom_command(commands[i][j], args, current_dir, home_dir, prev_dir, relative_dir, bg_flags[i][j]);
            }
        }

        check_bg_processes();
    }

    // Free the memory allocated for bg_flags
    for (int i = 0; i < max_comm_size; i++)
    {
        free(bg_flags[i]);
    }
    free(bg_flags);


    return 0;
}