#include "neonate.h"


pid_t find_recent_pid()
{
    DIR *dir;
    struct dirent *entry;
    pid_t recent_pid = -1;

    dir = opendir("/proc");
    if (dir == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (isdigit(entry->d_name[0]))
        {
            pid_t pid = atoi(entry->d_name);
            if (pid > recent_pid)
            {
                recent_pid = pid; // Keep track of the largest PID
            }
        }
    }
    closedir(dir);
    return recent_pid;
}
void set_nonblocking_terminal()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
void restore_terminal()
{
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
void neonate(int interval)
{
    set_nonblocking_terminal(); // Set terminal to non-blocking mode
    pid_t last_pid = -1;

    while (1)
    {
        struct timeval timeout;
        timeout.tv_sec = interval;
        timeout.tv_usec = 0;

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(STDIN_FILENO, &readfds))
        {
            char c = getchar();
            if (c == 'x' || c == 'X')
            {
                break; // Exit loop if 'x' or 'X' is pressed
            }
        }

        pid_t recent_pid = find_recent_pid();
        if (recent_pid != -1)
        {
            printf("%d\n", recent_pid);
            last_pid = recent_pid;
        }
    }

    restore_terminal(); // Restore terminal settings
    printf("Process terminated.\n");
}
