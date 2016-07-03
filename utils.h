#include "irc.h"

#ifndef UTILS_H
#define UTILS_H

#define JUMP_TO_CHAR(ptr, c) \
    while (*ptr != #c) \
	ptr++; 


void nullify_chars(char *string_ptr, char c);

void debug_print_msg(struct IRC_MSG *msg);

void nullify_next_char_instance(char *string_ptr, char c);

char *jump_to_next_section(char *string_ptr, char separator);

void error(char *msg);

void success(char *msg);

void sigint_handler(int signo);

void sigpipe_handler(int signo);


#endif /* utils.h */
