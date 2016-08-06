#include "irc.h"

#ifndef UTILS_H
#define UTILS_H

#define JUMP_TO_CHAR(ptr, c) \
    while (*ptr != #c) \
	ptr++; 


void debug_print_msg(struct IRC_MSG *msg);

void print_buf(char *buf, size_t len);

void error(char *msg);

void success(char *msg);

void sigint_handler(int signo);

void sigpipe_handler(int signo);


#endif /* utils.h */
