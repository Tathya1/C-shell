#include "log.h"

char *read_log(char log[no_of_logs][MAX_SIZE], int *pos)
{

    int cnt = 0;
    FILE *file = fopen(log_path, "r");
    if (file == NULL)
    {
        if (errno != ENOENT)
        {
            perror("Error opening log file");
        }
        cnt = 0;
        return NULL;
    }

    cnt = 0;
    while (cnt < no_of_logs && fgets(log[cnt], MAX_SIZE, file) != NULL)
    {

        cnt++;
    }
    // printf("%d\n",cnt);
    fclose(file);
    if (cnt == 0)
    {
        *pos = 0;
        return NULL;
    }
    // printf("%d\n",cnt);
    // printf("%d\n",*pos);


    if ((*pos) == 0)
    {
        (*pos) = cnt;
        return (cnt > 0) ? log[cnt - 1] : NULL; // Return the most recent command
    }
    else if ((*pos) > 0 && (*pos) <= cnt)
    {
        return log[cnt - *pos]; // Return the command at the desired position
    }
    return NULL;
}
void write_in_log(char log[no_of_logs][MAX_SIZE], int c)
{
    FILE *file = fopen(log_path, "w");
    if (file == NULL)
    {
        perror("Error writing to log file");
        return;
    }

    for (int i = 0; i < c; i++)
    {
        fprintf(file, "%s", log[i]);
    }

    fclose(file);
}
void store_in_log(char *command)
{
    char log[no_of_logs][MAX_SIZE];
    int c = 0;
    char *temp;
    if (strstr(command, "log") == NULL)
    {
        // printf("!");
        temp = read_log(log, &c);
        // if(temp == NULL)
        // printf("DUDE");
        if (temp != NULL && strcmp(command, temp) == 0)
        {
            return;
        }
        else
        {
            if (c == no_of_logs)
            {
                for (int i = 0; i < no_of_logs - 1; i++)
                {
                    strcpy(log[i], log[i + 1]);
                }
                strcpy(log[no_of_logs - 1], command);
            }
            else
            {
                strcpy(log[c], command);
                c++;
            }
            write_in_log(log, c);
        }
    }
}
void do_log(char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir)
{
    int i = 1;
    if (args[i] == NULL)
    {
        FILE *file = fopen(log_path, "r");
        if (file == NULL)
        {
            perror("Error opening log file");
            return;
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            printf("%s", buffer); // Print each line as it is read
        }

        fclose(file);
    }
    while (args[i] != NULL)
    {
        if (strcmp(args[i], "purge") == 0)
        {
            FILE *file = fopen(log_path, "w");
            if (file == NULL)
            {
                perror("Error purging log");
            }
            fclose(file);
            i++;
        }
        else if (strcmp(args[i], "execute") == 0)
        {
            i++;
            int pos = atoi(args[i]);
            // printf("%d\n", pos);
            char log[no_of_logs][MAX_SIZE];
            char temp[MAX_SIZE];

            // printf("%d",no_of_logs);
            if(read_log(log, &pos)==NULL)
            {
                printf("No such command in log\n");
                return;
            }
            strcpy(temp, read_log(log, &pos));
            store_in_log(temp);

            char commands1[max_comm_size][max_comm_size][max_comm_size];
            int rows1, cols1[max_comm_size];
            int **bg_flags1 = (int **)malloc(max_comm_size * sizeof(int *));

            if (bg_flags1 == NULL)
            {
                perror("Failed to allocate memory for bg_flags1");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < max_comm_size; i++)
            {
                bg_flags1[i] = (int *)malloc(max_comm_size * sizeof(int));

                if (bg_flags1[i] == NULL)
                {
                    perror("Failed to allocate memory for bg_flags1 row");
                    exit(EXIT_FAILURE);
                }
            }

            sep_input(temp, commands1, &rows1, cols1, bg_flags1,args,home_dir,prev_dir,relative_dir,current_dir);


            for (int i = 0; i < max_comm_size; i++)
            {
                free(bg_flags1[i]);
            }
            
            free(bg_flags1);
        }
        i++;
    }
}
