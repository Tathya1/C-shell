#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <grp.h>
#include <time.h>
#include <termios.h>
#include <sys/utsname.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


#define HOST "man.he.net"
#define PORT 80
#define MAX_SIZE 4097
#define max_comm_size 100
#define no_of_logs 15
#define MAX_BG_PROCESSES 1024
#define BUFFER_SIZE 4096
#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

#endif