#ifndef UTILS_H
#define UTILS_H

void error(char *msg);

void success(char *msg);

void sigint_handler(int signo);

void sigpipe_handler(int signo);


#endif /* utils.h */
