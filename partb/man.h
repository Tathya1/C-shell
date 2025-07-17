#ifndef MAN_H
#define MAN_H
#include "headers.h"

void fetch_man_page(const char *command_name);
void url_encode(const char *src, char *dest, size_t max_len);


#endif