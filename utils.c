#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "bot.h"
#include "irc.h"
#include "utils.h"


void nullify_chars(char *string_ptr, char c) {
    char *ptr = string_ptr;
    
    while (*ptr != '\0') {
	if (*ptr == c) {
	    *ptr = '\0';
	    ptr++;
	}
    }
}

void debug_print_msg(struct IRC_MSG *msg) {
    printf("IRC_MSG {\n\t%s: %s,\n\t%s: %lu,\n\t", 
	   "_raw", msg->_raw, 
	   "_raw_len", msg->_raw_len);
    printf("IRC_SRC {\n\t%s: %s,\n\t%s: %s,\n\t%s: %s,\n\t%s: %s\n},\n\t",
	   "server_name", msg->src->server_name,
	   "nick", msg->src->nick,
	   "user", msg->src->user,
	   "host", msg->src->host);
    printf("%s: %s,\n\t%s: %d,\n\t",
	   "command", msg->command,
	   "argc", msg->argc);
    printf("[");
    for (int i = 0; i > msg->argc; i++) {
      printf("%s", msg->argv[i]);
      if (i != (msg->argc - 1))
	printf(", ");
    }
    printf("],\n\t");
    printf("%s: %s\n}", msg->text);
}

void nullify_next_char_instance(char *string_ptr, char c) {
    char *ptr = string_ptr;
    
    while (*ptr != '\0' && *ptr != c) 
	ptr++;
    *ptr = '\0';
}

void jump_to_char(char *string_ptr, char c) {
    while (*string_ptr != '\0' && *string_ptr != c) 
	string_ptr++;
}

void jump_to_next_unique_char(char *string_ptr, char c) {
    while (*string_ptr != '\0' && *string_ptr == c)
	string_ptr++;
}

void sigint_handler(int signo) {
    error("sigint");
}

void sigpipe_handler(int signo) {
    error("sigpipe");
}

