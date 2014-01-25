#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "bot.h"
#include "utils.h"

void error(char *msg) {
    perror(msg);
    free_context();
    free(message_queue);
    exit(errno);
}

void success(char *msg) {
    printf("%s\n", msg);
    free_context();
    free(message_queue);
    exit(EXIT_SUCCESS);
}

void sigint_handler(int signo) {
    error("sigint");
}

void sigpipe_handler(int signo) {
    error("sigpipe");
}

