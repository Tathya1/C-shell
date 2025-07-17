#ifndef NEONATE_H
#define NEONATE_H

#include "headers.h"
pid_t find_recent_pid();
void set_nonblocking_terminal();
void restore_terminal();
void neonate(int interval);


#endif