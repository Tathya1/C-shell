#include "io.h"

char *trim_whitespace(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';

    return str;
}



void clean_command(char *command)
{
    int i = 0, j = 0;    // `i` for reading, `j` for writing to cleaned command
    int in_redirect = 0; // Flag to indicate if we're in a redirection block

    while (command[i] != '\0')
    {
        if (command[i] == '>')
        {
            i++; // Move past '>'
            if (command[i] == '>')
            {
                i++; // Move past the second '>' for append
            }
            while (command[i] == ' ')
                i++;
            while (command[i] != ' ' && command[i] != '\0')
                i++;
            in_redirect = 1; // Set the flag since we're in a redirection block
        }
        else if (command[i] == '<')
        {
            i++; // Move past '<'
            while (command[i] == ' ')
                i++;
            while (command[i] != ' ' && command[i] != '\0')
                i++;
            in_redirect = 1; // Set the flag since we're in a redirection block
        }
        else
        {
            if (!in_redirect)
            {
                command[j++] = command[i];
            }
            i++;
        }

        if (in_redirect && command[i] != ' ' && command[i] != '\0')
        {
            in_redirect = 0;
        }
    }

    command[j] = '\0';
}

void sep_input(char *input, char commands[max_comm_size][max_comm_size][max_comm_size], int *rows, int *cols, int **bg_flags, char **args, char *home_dir, char *prev_dir, char *relative_dir, char *current_dir)
{
    char *comm1, *comm2;
    char input1[MAX_CMD_LEN];
    char temp1[MAX_CMD_LEN];
    int x = 1000, y = 1000;
    char *sptr1, *sptr2;
    int row = 0;
    int c = 0, c1 = 0;
    trim_whitespace(input);
    // printf("%s\n", input);
    if (input[0] == '|' || input[strlen(input) - 1] == '|')
    {
        printf("Error: Invalid syntax. Command starts or ends with a pipe.\n");
        return 1; // Exit with an error
    }
    comm1 = strtok_r(input, ";", &sptr1);
    while (comm1 != NULL && row < MAX_SIZE)
    {
        int col = 0;
        int found_background = 0;
        int isPipe = 0, isIO = 0;

        int len = strlen(comm1);
        for (int i = 0; i < len; i++)
        {
            if (comm1[i] == '&')
            {
                c++;
            }
            if (comm1[i] == '|')
            {
                isPipe = 1;
            }
            if (comm1[i] == '<' || comm1[i] == '>')
            {
                isIO = 1;
            }
        }

        if (isIO && !isPipe)
        {
            strcpy(input1, comm1);

            int len1 = strlen(input1);
            if (input1[len1 - 1] == '\n')
            {
                input1[len1 - 1] = '\0';
            }
            char comm[6000];
            char temp[6000];
            strcpy(temp, input1);
            strcpy(comm, input1);
            char *in_file = strstr(input1, "<");
            if (in_file != NULL)
            {
                *in_file = '\0'; // Terminate the command at '<'
                in_file++;       // Move past the '<'
                in_file = strtok(in_file, " ");
                int fd_in = open(in_file, O_RDONLY);
                if (fd_in < 0)
                {
                    perror("Input file open error");
                    exit(EXIT_FAILURE);
                }
                x = dup(0);
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            char *out_file = strstr(comm, ">");
            int append_mode = 0;

            if (out_file != NULL)
            {
                if (*(out_file + 1) == '>')
                {
                    append_mode = 1; // It's an append
                    out_file += 2;   // Skip ">>"
                }
                else
                {
                    out_file++; // Skip ">"
                }

                out_file = strtok(out_file, " ");
                int fd_out;

                if (append_mode)
                {
                    fd_out = open(out_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                }
                else
                {
                    fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }

                if (fd_out < 0)
                {
                    perror("Output file open error");
                    exit(EXIT_FAILURE);
                }
                y = dup(1);
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
        }
        else if (isIO || isPipe)
        {

            strcpy(input1, comm1);

            int len1 = strlen(input1);
            if (input1[len1 - 1] == '\n')
            {
                input1[len1 - 1] = '\0';
            }
            char *commands[100]; // Array to hold commands split by pipes
            int num_commands = 0;
            char *comm = input1;
            char *saveptr;

            for (int i = 0; i < strlen(comm) - 1; i++)
            {
                if (comm[i] == '|' && comm[i + 1] == '|')
                {
                    comm[i - 1] = '\0';
                    break;
                }
                else if (comm[i] == '&' || comm[i] == '|')
                {
                    int next_non_space = i + 1;
                    while (isspace(comm[next_non_space]))
                        next_non_space++; // Skip spaces after current character

                    if ((comm[i] == '&' && comm[next_non_space] == '|') || (comm[i] == '|' && comm[next_non_space] == '&'))
                    {
                        printf("Error: Invalid syntax. Misplaced '&' and '|'.\n");
                        return 1; // Invalid combination found
                    }

                    if (comm[i] == '|' && comm[next_non_space] == '|')
                    {
                        printf("Error: Invalid syntax. Two consecutive pipes.\n");
                        return 1; // Invalid combination found
                    }
                }
            }

            while ((commands[num_commands] = strtok_r(comm, "|", &saveptr)) != NULL)
            {
                commands[num_commands] = trim_whitespace(commands[num_commands]);

                if (strlen(commands[num_commands]) == 0)
                {
                    printf("Error: Invalid syntax. Empty command between pipes.\n");
                    return 1; // Exit with an error
                }

                if (commands[num_commands][0] == '&' || (strchr(commands[num_commands], '&') != NULL && commands[num_commands][strlen(commands[num_commands]) - 1] == '|'))
                {
                    printf("Error: Invalid syntax. Misplaced '&' character.\n");
                    return 1; // Exit with an error
                }

                // printf("Command %d: %s (Length: %lu)\n", num_commands, commands[num_commands], strlen(commands[num_commands]));
                comm = NULL; // Reset comm to NULL for subsequent strtok_r calls
                num_commands++;
            }

            int pipefd[2], fd = 0; // 'fd' will hold the input for the next command (initially 0, i.e., stdin)

            for (int i = 0; i < num_commands; i++)
            {
                char *saveptr2;
                int status;
                char temp[1000];

                pipe(pipefd);
                pid_t pid = fork();
                if (pid == 0) // Child process
                {
                    setpgid(0, 0); // Set process group ID (optional, for job control)

                    dup2(fd, STDIN_FILENO); // Get input from the previous command or stdin

                    if (i < num_commands - 1)
                    {
                        dup2(pipefd[1], STDOUT_FILENO); // Output to next command
                    }

                    close(pipefd[0]);
                    close(pipefd[1]);
                    strcpy(temp, commands[i]);
                    char *out_file = strstr(commands[i], ">");
                    char *in_file = strstr(commands[i], "<");
                    int append_mode = 0;

                    if (in_file != NULL)
                    {
                        *in_file = '\0';                // Terminate the command at '<'
                        in_file++;                      // Move past the '<'
                        in_file = strtok(in_file, " "); // Get the filename
                        int fd_in = open(in_file, O_RDONLY);
                        if (fd_in < 0)
                        {
                            perror("Input file open error");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in); // Close after redirection
                    }

                    if (out_file != NULL)
                    {
                        if (*(out_file + 1) == '>')
                        {
                            append_mode = 1; // It's an append
                            out_file += 2;   // Skip ">>"
                        }
                        else
                        {
                            out_file++; // Skip ">"
                        }

                        out_file = strtok(out_file, " "); // Get the filename
                        int fd_out;

                        if (append_mode)
                        {
                            fd_out = open(out_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        }
                        else
                        {
                            fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        }

                        if (fd_out < 0)
                        {
                            perror("Output file open error");
                            exit(EXIT_FAILURE);
                        }

                        dup2(fd_out, STDOUT_FILENO); // Redirect output
                        close(fd_out);               // Close after redirection
                    }

                    clean_command(temp);

                    int len = strlen(temp);
                    c = 0;
                    c1 = 0;
                    for (int i = 0; i < len; i++)
                    {
                        if (temp[i] == '&')
                        {
                            c++;
                        }
                    }

                    comm2 = strtok_r(temp, "&", &sptr2);
                    while (comm2 != NULL && col < MAX_SIZE)
                    {
                        comm2 = trim_whitespace(comm2);

                        if (strlen(comm2) > 0)
                        {

                            c1++;
                            if (c1 <= c)
                            {
                                bg_flags[row][col] = 1;
                            }
                            else
                            {
                                bg_flags[row][col] = 0;
                            }
                            strcpy(temp1, comm2);
                            // printf("%s\n",comm2);
                            clean_command(temp1);
                            handle_custom_command(temp1, args, current_dir, home_dir, prev_dir, relative_dir, bg_flags[row][col]);
                            if (x != 1000)
                            {
                                dup2(x, 0);
                                close(x);
                            }
                            if (y != 1000)
                            {
                                dup2(y, 1);
                                close(y);
                            }
                            isPipe = 0;
                            isIO = 0;

                            col++;
                        }

                        comm2 = strtok_r(NULL, "&", &sptr2);
                    }
                    cols[row] = col;
                    row++;

                    exit(EXIT_SUCCESS);
                }
                else if (pid < 0) // Fork failure
                {
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                }
                close(pipefd[1]);
                fd = pipefd[0];
            }

            for (int i = 0; i < num_commands; i++)
            {

                wait(NULL);
            }
            isPipe = 0;
            isIO = 0;

            comm1 = strtok_r(NULL, ";", &sptr1);
            continue;
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
                strcpy(temp1, commands[row][col]);
                clean_command(temp1);
                handle_custom_command(temp1, args, current_dir, home_dir, prev_dir, relative_dir, bg_flags[row][col]);
                if (x != 1000)
                {
                    dup2(x, 0);
                    close(x);
                }
                if (y != 1000)
                {
                    dup2(y, 1);
                    close(y);
                }
                isPipe = 0;
                isIO = 0;

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
