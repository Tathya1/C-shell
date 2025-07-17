#ifndef SEEK_H
#define SEEK_H

#include "headers.h"

void search_directory(const char *base_path, const char *target, int search_files, int search_dirs, char **result, int *count);
void seek_command(char *args[],char* home_dir,char* prev_dir);
void print_in_color(const char *path, int is_directory);

#endif