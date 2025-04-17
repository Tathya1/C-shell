#include "proclore.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <grp.h>
#include <time.h>
#include <sys/utsname.h>
#include <signal.h>

    void proclore(int pid) {
    char proc_path[BUFFER_SIZE];
    char status_file[BUFFER_SIZE];
    char stat_file[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char exec_path[BUFFER_SIZE];

    FILE *file;
    int foreground_group_id;
    // Set up the path to the process' /proc/[pid] directory
    sprintf(proc_path, "/proc/%d", pid);
    
    // Set up the path to the status file
    sprintf(status_file, "%s/status", proc_path);
    sprintf(stat_file, "%s/stat", proc_path);

    // Open the status file
    file = fopen(status_file, "r");
    if (!file) {
        perror("Could not open status file");
        return;
    }

    // Variables to hold process information
    char process_status[3];
    // char exec_path[BUFFER_SIZE];
    int process_group = 0;
    unsigned long vsize = 0;

    // Read the status file line by line
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "State:", 6) == 0) {
            sscanf(buffer, "State:\t%s", process_status);
        } else if (strncmp(buffer, "Tgid:", 5) == 0) {
            sscanf(buffer, "Tgid:\t%d", &process_group);
        } else if (strncmp(buffer, "VmSize:", 7) == 0) {
            sscanf(buffer, "VmSize:\t%lu", &vsize);
        }
    }

    fclose(file);

    // Get the executable path
    sprintf(exec_path, "%s/exe", proc_path);
    ssize_t len = readlink(exec_path, exec_path, sizeof(exec_path) - 1);
    if (len != -1) {
        exec_path[len] = '\0'; // Null-terminate the path

        // Convert to absolute path
        char absolute_path[4097];
        if (realpath(exec_path, absolute_path) != NULL) {
            strcpy(exec_path, absolute_path);
        }
    } else {
        strcpy(exec_path, "Unknown");
    }



    printf("%s\n",stat_file);
    file = fopen(stat_file, "r");
    if (file) {
        int stat_pid, ppid, pgrp, session, tty_nr, tpgid;
        char comm[BUFFER_SIZE], state;
        
        fscanf(file, "%d %s %c %d %d %d %d %d %d", &stat_pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &foreground_group_id);
        fclose(file);

        // Check if the process is in the foreground
        if (pgrp == tpgid) {
            strcat(process_status, "+");
        }
    }

    // Print the collected information
    printf("pid : %d\n", pid);
    printf("process status : %s\n", process_status);
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %lu kB\n", vsize);
    printf("executable path : %s\n", exec_path);
}
void handle_proclore(char *args[]) {
    int pid;

    if (args[1] == NULL) {
        // No argument provided, print information for the shell itself
        pid = getpid();
    } else {
        // Argument provided, print information for the specified process
        pid = atoi(args[1]);
    }

    proclore(pid);
}