#ifndef SPLIT_H
#define SPLIT_H

#include "headers.h"
#include "process.h"
#include "log.h"
#include "hop.h"
#include "reveal.h"
#include "proclore.h"
#include "seek.h"
#include "neonate.h"
#include "man.h"


void seek1(const char *dirname);
void hop_seek(const char *dirname);
void init_shell();
void handleIO_redirection(char *input);
void execute_command1(char *command);
void tokenize_command(char *command, char *args[]);
char *trim_whitespace(char *str);

void handle_custom_command(char *command, char *args[], char *current_dir, char *home_dir, char *prev_dir, char *relative_dir, int is_background);

#endif