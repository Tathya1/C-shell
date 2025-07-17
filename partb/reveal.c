#include "reveal.h"
extern int alphasort(const struct dirent **a, const struct dirent **b);
void print_permissions(mode_t mode)
{
    char label[11];

    if (S_ISDIR(mode))
    {
        label[0] = 'd';
    }
    else
    {
        label[0] = '-';
    }

    if (mode & S_IRUSR)
    {
        label[1] = 'r';
    }
    else
    {
        label[1] = '-';
    }

    if (mode & S_IWUSR)
    {
        label[2] = 'w';
    }
    else
    {
        label[2] = '-';
    }

    if (mode & S_IXUSR)
    {
        label[3] = 'x';
    }
    else
    {
        label[3] = '-';
    }

    if (mode & S_IRGRP)
    {
        label[4] = 'r';
    }
    else
    {
        label[4] = '-';
    }

    if (mode & S_IWGRP)
    {
        label[5] = 'w';
    }
    else
    {
        label[5] = '-';
    }

    if (mode & S_IXGRP)
    {
        label[6] = 'x';
    }
    else
    {
        label[6] = '-';
    }

    if (mode & S_IROTH)
    {
        label[7] = 'r';
    }
    else
    {
        label[7] = '-';
    }

    if (mode & S_IWOTH)
    {
        label[8] = 'w';
    }
    else
    {
        label[8] = '-';
    }

    if (mode & S_IXOTH)
    {
        label[9] = 'x';
    }
    else
    {
        label[9] = '-';
    }

    label[10] = '\0';

    printf("%s ", label);
}

void reveal_command(int show_hidden, int show_long, char *path, char *home_dir, char *prev_dir)
{
    char new_dir[MAX_SIZE];
    struct dirent **info;
    int n;

    if (strcmp(path, "~") == 0)
    {
        strcpy(new_dir, home_dir);
    }
    else if (strcmp(path, "/") == 0)
    {
        strcpy(new_dir, "/");
    }
    else if (strcmp(path, "-") == 0)
    {
        if (strcmp(prev_dir, "") == 0)
            printf("OLDPWD not set\n");
        else
            strcpy(new_dir, prev_dir);
    }
    else if (strcmp(path, ".") == 0)
    {
        getcwd(new_dir, sizeof(new_dir));
    }
    else if (strcmp(path, "..") == 0)
    {
        getcwd(new_dir, sizeof(new_dir));
        strcat(new_dir, "/..");
    }
    else if (path[0] == '~')
    {
        snprintf(new_dir, sizeof(new_dir), "%s%s", home_dir, path + 1);
    }
    else
    {
        strcpy(new_dir, path);
    }

    struct stat statbuf;
    if (stat(new_dir, &statbuf) == -1)
    {
        perror("stat");
        return;
    }

    if (S_ISDIR(statbuf.st_mode)) // If the target is a directory
    {
        n = scandir(new_dir, &info, NULL, alphasort);
        if (n < 0)
        {
            perror("scandir");
            return;
        }
        long long int total = 0;

        for (int i = 0; i < n; i++)
        {
            struct dirent *entry = info[i];

            if (!show_hidden && entry->d_name[0] == '.')
            {
                continue;
            }

            char filepath[MAX_SIZE];
            snprintf(filepath, sizeof(filepath), "%s/%s", new_dir, entry->d_name);
            if (stat(filepath, &statbuf) == -1)
            {
                perror("stat");
                continue;
            }

            total += statbuf.st_blocks;
        }

        // Print total number of blocks
        if (show_long)
        {
            printf("total %lld\n", total / 2); // Dividing by 2 to match 1024-byte units.
        }

        for (int i = 0; i < n; i++)
        {
            struct dirent *entry = info[i];

            if (!show_hidden && entry->d_name[0] == '.')
            {
                free(entry);
                continue;
            }

            char filepath[MAX_SIZE];
            snprintf(filepath, sizeof(filepath), "%s/%s", new_dir, entry->d_name);
            if (stat(filepath, &statbuf) == -1)
            {
                perror("stat");
                free(entry);
                continue;
            }
            if (show_long)
            {
                print_permissions(statbuf.st_mode);
                printf("%ld ", statbuf.st_nlink);
                printf("%s ", getpwuid(statbuf.st_uid)->pw_name);
                printf("%s ", getgrgid(statbuf.st_gid)->gr_name);
                printf("%ld ", statbuf.st_size);
                char timebuf[80];
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
                printf("%s ", timebuf);
            }

            if (S_ISDIR(statbuf.st_mode))
            {
                printf("\033[1;34m%s\033[0m\n", entry->d_name); // Blue for directories
            }
            else if (S_ISREG(statbuf.st_mode) && (statbuf.st_mode & S_IXUSR))
            {
                printf("\033[1;32m%s\033[0m\n", entry->d_name); // Green for executables
            }
            else
            {
                printf("%s\n", entry->d_name); // Default color for regular files
            }

            free(entry);
        }

        free(info);
    }
    else if (S_ISREG(statbuf.st_mode)) // If the target is a regular file
    {
        if (show_long)
        {
            print_permissions(statbuf.st_mode);
            printf("%ld ", statbuf.st_nlink);
            printf("%s ", getpwuid(statbuf.st_uid)->pw_name);
            printf("%s ", getgrgid(statbuf.st_gid)->gr_name);
            printf("%ld ", statbuf.st_size);
            char timebuf[80];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
            printf("%s ", timebuf);
        }
        printf("%s\n", new_dir);
    }
}

void process_command(char *args[], char *home_dir, char *prev_dir)
{
    if (args[0] == NULL)
        return;

    if (strcmp(args[0], "reveal") == 0)
    {
        int show_hidden = 0, show_long = 0;
        char *path = NULL;
        for (int i = 1; args[i] != NULL; i++)
        {
            if (args[i][0] == '-')
            {
                if (args[i][1] == '\0')
                {
                    if (strcmp(prev_dir, "") == 0)
                    {
                        printf("OLDPWD not set\n");
                        return;
                    }
                    else
                    {
                        path = args[i];
                    }
                }
                for (int j = 1; args[i][j] != '\0'; j++)
                {
                    if (args[i][j] == 'a')
                        show_hidden = 1;
                    else if (args[i][j] == 'l')
                        show_long = 1;
                }
            }
            else
            {
                path = args[i];
            }
        }
        if (!path)
        {
            path = ".";
        }
        reveal_command(show_hidden, show_long, path, home_dir, prev_dir);
    }
    else
    {
        printf("ERROR: Command not recognized\n");
    }
}
