#ifndef HOP_H
#define HOP_H

#include "headers.h"

void hop(char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir);
void rel_dir(char *relative_dir, const char *current_dir, const char *home_dir);
void print_permissions(mode_t mode);

#endif