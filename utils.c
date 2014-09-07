#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "bot.h"
#include "utils.h"

void sigint_handler(int signo) {
    error("sigint");
}

void sigpipe_handler(int signo) {
    error("sigpipe");
}

