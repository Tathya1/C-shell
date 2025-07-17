#ifndef IO_H
#define IO_H

#include "headers.h"
void clean_command(char *command);
void sep_input(char *input, char commands[max_comm_size][max_comm_size][max_comm_size], int *rows, int *cols, int **bg_flags, char **args, char *home_dir, char *prev_dir, char *relative_dir, char *current_dir);
char *trim_whitespace(char *str);

#endif