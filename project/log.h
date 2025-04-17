#ifndef LOG_H
#define LOG_H

#include "headers.h"
#include "split.h"

extern char log_path[MAX_SIZE];

char *read_log(char log[no_of_logs][MAX_SIZE], int *pos);
void write_in_log(char log[no_of_logs][MAX_SIZE], int c);
void store_in_log(char *command);
void do_log(char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir);

#endif