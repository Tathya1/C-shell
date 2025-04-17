#ifndef SPLIT_H
#define SPLIT_H

#include "headers.h"
#include "process.h"
#include "log.h"
#include "hop.h"
#include "reveal.h"
#include "proclore.h"
#include "seek.h"

void tokenize_command(char *command, char *args[]);
char *trim_whitespace(char *str);
void sep_input(char *input, char commands[max_comm_size][max_comm_size][max_comm_size], int *rows, int *cols, int **bg_flags);
void handle_custom_command(char *command, char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir, int is_background);

#endif