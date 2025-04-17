#include "split.h"

void tokenize_command(char *command, char *args[])
{
    int i = 0;
    char *token;

    // Use strtok to split the command by spaces
    token = strtok(command, " \t\n");
    while (token != NULL && i < max_comm_size - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}
char *trim_whitespace(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

void sep_input(char *input, char commands[max_comm_size][max_comm_size][max_comm_size], int *rows, int *cols, int **bg_flags)
{
    char *comm1, *comm2;
    char *sptr1, *sptr2;
    int row = 0;
    int c = 0, c1 = 0;
    comm1 = strtok_r(input, ";", &sptr1);
    while (comm1 != NULL && row < MAX_SIZE)
    {
        int col = 0;
        int found_background = 0;

        // Check if there's an '&' after the last command in this group
        int len = strlen(comm1);
        c = 0;
        c1 = 0;
        for (int i = 0; i < len; i++)
        {
            if (comm1[i] == '&')
            {
                c++;
            }
        }
        // printf("%d ",c);
        if (len > 0 && comm1[len - 1] == '&')
        {
            found_background = 1;
            comm1[len - 1] = '\0'; // Remove the trailing '&'
        }

        comm2 = strtok_r(comm1, "&", &sptr2);
        while (comm2 != NULL && col < MAX_SIZE)
        {
            comm2 = trim_whitespace(comm2);

            if (strlen(comm2) > 0)
            {
                strncpy(commands[row][col], comm2, max_comm_size - 1);
                commands[row][col][max_comm_size - 1] = '\0'; // Ensure null termination
                c1++;
                if (c1 <= c)
                {
                    bg_flags[row][col] = 1;


                }
                else
                {
                    bg_flags[row][col] = 0;

                }

                col++;
            }

            comm2 = strtok_r(NULL, "&", &sptr2);
        }
        cols[row] = col;
        row++;

        comm1 = strtok_r(NULL, ";", &sptr1);
    }

    *rows = row;

}


void handle_custom_command(char *command, char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir, int is_background)
{
    tokenize_command(command, args);
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
        seek_command(args,home_dir,prev_dir);

    }
    else
    {
        check_bg_processes();
        execute_command(args, is_background);
    }
}
