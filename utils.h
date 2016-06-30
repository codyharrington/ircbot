#ifndef UTILS_H
#define UTILS_H

void nullify_chars(char *string_ptr, char c);

void nullify_next_char_instance(char *string_ptr, char c);

void jump_to_char(char *string_ptr, char c);

void jump_to_next_unique_char(char *string_ptr, char c);

void error(char *msg);

void success(char *msg);

void sigint_handler(int signo);

void sigpipe_handler(int signo);


#endif /* utils.h */
