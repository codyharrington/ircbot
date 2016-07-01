#include <stdio.h>
#include "bot.h"

#ifndef IRC_H
#define IRC_H

#define IRC_MESSAGE_SIZE 513
#define IRC_MSG_TERMINATOR "\r\n"
#define READ_BUF_SIZE_BYTES 1024

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

int msg_is_valid(struct IRC_MSG *msg);

void handle_irc_command(struct IRC_CTX *ctx);

void parse_src(struct IRC_SRC *src, char *ptr);

void parse_command(char *command, char *ptr);

void parse_params(int *argc, char **argv, char *ptr);

void parse_text(char *text, char *ptr);

void parse_irc_msg(struct IRC_MSG *msg);

void parse_read_buffer(struct IRC_CTX *ctx, char *read_buf, size_t *remainder);

#endif /* irc.h */