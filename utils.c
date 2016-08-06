#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "include/bot.h"
#include "include/irc.h"
#include "include/utils.h"

void debug_print_msg(struct IRC_MSG *msg) {
    printf("IRC_MSG: {\n\t %s: ", "_raw");
    print_buf(msg->_raw, msg->_raw_len);
    printf(",\n\t%s: %lu,\n\t", 
	   "_raw_len", msg->_raw_len);
    printf("IRC_SRC: {\n\t\t%s: %s,\n\t\t%s: %s,\n\t\t%s: %s,\n\t\t%s: %s\n\t},\n\t",
	   "server_name", msg->src->server_name,
	   "nick", msg->src->nick,
	   "user", msg->src->user,
	   "host", msg->src->host);
    printf("%s: %s,\n\t%s: %d,\n\t",
	   "command", msg->command,
	   "argc", msg->argc);
    printf("argv: [");
    for (int i = 0; i < msg->argc; i++) {
      printf("%s", msg->argv[i]);
      if (i != (msg->argc - 1))
	printf(", ");
    }
    printf("],\n\t");
    printf("%s: %s,\n\t%s: %d\n}\n",
	   "text", msg->text,
	   "valid", msg->valid
	);
}

void print_buf(char *buf, size_t len) {
   for (int i = 0; i < len; i++) {
        switch (buf[i]) {
         case '\n': 
                fputs("\\n", stdout);
                break;
         case '\r': 
                fputs("\\r", stdout);
                break;
         case '\0': 
                fputs("\\0", stdout);
                break;
         case '\t':
                fputs("\\t", stdout);
                break;
         default:
                putchar(buf[i]);
        }
   }
}

void sigint_handler(int signo) {
    error("sigint");
}

void sigpipe_handler(int signo) {
    error("sigpipe");
}

void error(char *msg) {
  printf(msg);
}

void success(char *msg) {
  printf(msg);
}

