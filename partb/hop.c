#include "hop.h"

void rel_dir(char *relative_dir, const char *current_dir, const char *home_dir)
{
    if (strcmp(current_dir, home_dir) == 0)
    {
        strcpy(relative_dir, "~");
        return;
    }
    else if (strncmp(current_dir, home_dir, strlen(home_dir)) == 0)
    {

        sprintf(relative_dir, "~%s", current_dir + strlen(home_dir));
    }
    else
    {
        strcpy(relative_dir, current_dir);
    }
}

void hop(char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir)
{
    char new_dir[MAX_SIZE];
    strcpy(new_dir, current_dir);

    int i = 1;
    if (args[i] == NULL)
    {
        printf("%s\n", home_dir);
        chdir(home_dir);

        strcpy(prev_dir, current_dir);

        return;
    }
    else
    {
        while (args[i] != NULL)
        {
            // printf("%s",args[i]);
            if (strcmp(args[i], "~") == 0)
            {
                strcpy(new_dir, home_dir);
            }
            else if (strcmp(args[i], "/") == 0)
            {
                strcpy(new_dir, "/");
            }
            else if (strcmp(args[i], "-") == 0)
            {
                if (strcmp(prev_dir, "") == 0)
                    printf("OLDPWD not set\n");
                else
                    strcpy(new_dir, prev_dir);
            }
            else if (strcmp(args[i], ".") == 0)
            {
                rel_dir(relative_dir, new_dir, home_dir);
                printf("%s\n", relative_dir);
                i++;
                continue;
            }
            else if (strcmp(args[i], "..") == 0)
            {
                if (chdir("..") != 0)
                {
                    perror("chdir");
                    return;
                }
                if (getcwd(new_dir, MAX_SIZE) == NULL)
                {
                    perror("getcwd");
                    return;
                }
                printf("%s\n", new_dir);
                i++;
                continue;
            }
            else if (strncmp(args[i], "~/", 2) == 0)
            {
                strcpy(new_dir, home_dir);
                strcat(new_dir, args[i] + 1);
            }
            else if (strncmp(args[i], "./", 2) == 0)
            {
                // strcpy(new_dir, current_dir);
                strcat(new_dir, args[i] + 1);
            }
            else if (strncmp(args[i], "../", 3) == 0)
            {
                if (chdir("..") != 0)
                {
                    perror("chdir");
                    return;
                }
                if (getcwd(new_dir, MAX_SIZE) == NULL)
                {
                    perror("getcwd");
                    return;
                }
                // printf("%s\n", new_dir);
                strcat(new_dir, args[i] + 2);
            }
            else if (strncmp(args[i], "/", 1) == 0)
            {
                // strcpy(new_dir, current_dir);
                strcpy(new_dir, args[i]);
            }
            else
            {
                // strcpy(new_dir, current_dir);
                strcat(new_dir, "/");
                strcat(new_dir, args[i]);
            }
            printf("%s\n", new_dir);
            if (chdir(new_dir) != 0)
            {
                perror("chdir");
                return;
            }

            i++;
        }

        strcpy(prev_dir, current_dir);

    }
}
