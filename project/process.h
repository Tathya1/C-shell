#ifndef PROCESS_H
#define PROCESS_H

#include "headers.h"
#include "split.h"
#include "act.h"
#include "signals.h"
#include "fgbg.h"
#include "io.h"


extern char concatenated_commands[256 * 100];
extern long total_duration;
extern int commands_logged;
void tokenize_command1(char *command, char *args[]);

void push_back(int pid);
void add_fg_process(pid_t pid, char *name, int status);
void push_fore(pid_t pid);
void delete_process(pid_t pid);
void update_process_status(pid_t pid, int status);
void sigchld_handler(int sig);
void add_bg_process(pid_t pid, char *name,int status);
void check_bg_processes();
void update_process_state(int pid);
void ping(pid_t pid, int signal_num);
void execute_command(char* command,char *args[], int is_background);

#endif