#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "bot.h"
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

