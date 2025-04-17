
#include "seek.h"

void search_directory(const char *base_path, const char *target, int search_files, int search_dirs, char **result, int *count)
{
    struct dirent *dp;
    DIR *dir = opendir(base_path);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, dp->d_name);
        // printf("%s ",base_path);
        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }

        if ((search_files && S_ISREG(statbuf.st_mode) && strncmp(dp->d_name, target, strlen(target)) == 0) ||
            (search_dirs && S_ISDIR(statbuf.st_mode) && strncmp(dp->d_name, target, strlen(target)) == 0))
        {
            result[*count] = strdup(full_path);
            (*count)++;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            search_directory(full_path, target, search_files, search_dirs, result, count);
        }
    }

    closedir(dir);
}

void seek_command(char *args[], char *home_dir, char *prev_dir)
{
    int search_files = 1, search_dirs = 1, execute_flag = 0, home_flag = 0, prev_flag = 0;
    char *target = NULL, *directory = ".";
    char *result[1024];
    int count = 0;

    // Parse flags and arguments from args
    for (int i = 1; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "-d") == 0)
        {
            search_files = 0;
        }
        else if (strcmp(args[i], "-f") == 0)
        {
            search_dirs = 0;
        }
        else if (strcmp(args[i], "-e") == 0)
        {
            execute_flag = 1;
        }
        else if (target == NULL)
        {
            target = args[i];
        }
        else
        {
            directory = args[i];
        }
    }

    char expanded_dir[1024];
    if (directory[0] == '~')
    {
        home_flag = 1;

        snprintf(expanded_dir, sizeof(expanded_dir), "%s%s", home_dir, directory + 1);

        directory = expanded_dir;
        // printf("%s ", expanded_dir);
        // strncpy(directory,expanded_dir,strlen(home_dir));
    }
    char temp[1024];
    if (directory[0] == '-')
    {
        prev_flag = 1;
        if (strcmp(prev_dir, "") == 0)
        {
            printf("OLDPWD not set\n");
            if (getcwd(temp, MAX_SIZE) == NULL)
            {
                perror("getcwd");
                return;
            }
            strcpy(prev_dir, temp);
            return;
        }
        else
            strcpy(temp, prev_dir);
        snprintf(expanded_dir, sizeof(expanded_dir), "%s%s", temp, directory + 1);
        directory = expanded_dir;
        printf("%s ", expanded_dir);
    }

    // Validate flags
    if (search_files == 0 && search_dirs == 0)
    {
        printf("Invalid flags!\n");
        return;
    }

    if (target == NULL)
    {
        printf("No target specified!\n");
        return;
    }

    search_directory(directory, target, search_files, search_dirs, result, &count);

    // Handle the `-e` flag and output results
    if (count == 0)
    {
        printf("No match found!\n");
    }
    else if (execute_flag && count == 1)
    {
        struct stat statbuf;

        if (stat(result[0], &statbuf) == -1)
        {
            perror("stat");
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            char s7[5000] = ".";
            if (home_flag || prev_flag)
                strcat(s7, result[0] + strlen(expanded_dir));
            else
                strcat(s7, result[0] + strlen(directory));
            print_in_color(s7, S_ISDIR(statbuf.st_mode));
            if (access(result[0], R_OK) == 0)
            {
                FILE *file = fopen(result[0], "r");
                if (file)
                {
                    char line[256];
                    while (fgets(line, sizeof(line), file))
                    {
                        printf("%s", line);
                    }
                    fclose(file);
                }
                printf("\n");
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            char s7[5000] = ".";
            if (home_flag || prev_flag)
                strcat(s7, result[0] + strlen(expanded_dir));
            else
                strcat(s7, result[0] + strlen(directory));
            print_in_color(s7, S_ISDIR(statbuf.st_mode));
            if (access(result[0], X_OK) == 0)
            {
                if (getcwd(temp, MAX_SIZE) == NULL)
                {
                    perror("getcwd");
                    return;
                }
                if (chdir(result[0]) == 0)
                {

                    strcpy(prev_dir, temp);
                }
                else
                {
                    perror("chdir");
                }
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
    }
    else
    {
        for (int i = 0; i < count; i++)
        {
            struct stat statbuf;
            if (stat(result[i], &statbuf) == -1)
            {
                perror("stat");
            }
            else
            {
                char s6[5000] = ".";
                if (home_flag || prev_flag)
                    strcat(s6, result[i] + strlen(expanded_dir));
                else
                    strcat(s6, result[i] + strlen(directory));

                print_in_color(s6, S_ISDIR(statbuf.st_mode));
            }
            free(result[i]); // Free allocated memory
        }
    }
}

void print_in_color(const char *path, int is_directory)
{
    if (is_directory)
    {
        printf("\033[1;34m%s\033[0m\n", path); // Blue for directories
    }
    else
    {
        printf("\033[1;32m%s\033[0m\n", path); // Green for files
    }
}
