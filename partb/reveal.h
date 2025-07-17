#ifndef REVEAL_H
#define REVEAL_H

#include "headers.h"

void print_permissions(mode_t mode);
void reveal_command(int show_hidden, int show_long, char *path, char *home_dir, char *prev_dir);
void process_command(char *args[], char *home_dir, char *prev_dir);

#endif