#include <stdio.h>

#ifndef IRC_H
#define IRC_H

#define IRC_MESSAGE_SIZE 512

struct IRC_SRC {
	char *server_name;
	char *nick;
	char *user;
	char *host;
};

struct IRC_MSG {
	char *_raw;
	size_t _raw_len;
	struct IRC_SRC *src;
	char *command;
	int argc;
	char **argv;
	char *text;
};

struct IRC_MSG *init_msg();

void free_msg(struct IRC_MSG *msg);

struct IRC_MSG *create_message(char *raw, size_t raw_len);

void handle_irc_command(struct IRC_CTX *ctx);

void parse_src(struct IRC_SRC *src, char *ptr);

void parse_command(char *command, char *ptr);

void parse_params(int *argc, char **argv, char *message, char *ptr);

void parse_text(char *text, char *ptr);

void parse_irc_msg(struct IRC_MSG *msg);

void parse_read_buffer(struct IRC_CTX *ctx, char *read_buf, size_t *remainder);

#endif /* irc.h */