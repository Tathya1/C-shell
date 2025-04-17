#ifndef PROCESS_H
#define PROCESS_H

#include "headers.h"


extern char concatenated_commands[256 * 100];
extern long total_duration;
extern int commands_logged;

void sigchld_handler(int sig);
void add_bg_process(pid_t pid, char *name);
void check_bg_processes();
void execute_command(char *args[], int is_background);

#endif