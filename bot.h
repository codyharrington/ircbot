#ifndef BOT_H
#define BOT_H

#include <netdb.h>
#include <semaphore.h>

#define IRC_MESSAGE_SIZE 513
#define PORT_STR_LEN 5
#define IRC_MSG_TERMINATOR "\r\n"
#define READ_BUF_SIZE_BYTES 1024
#define WRITE_BUF_SIZE_BYTES 1024

#define JUMP_TO_SPACE(str) \
	while (*str != ' ') \
		str++;

#define NULLIFY_SPACES(str) \
	while (*str == ' ') { \
		*str = '\0'; \
		str++; \
	} \


struct IRC_CTX {
	struct sockaddr servaddr;
	socklen_t servaddr_len;
	char *servaddr_str;
	char *channel; 
	char *nick;
	int sockfd;
	int port;
	struct IRC_MSG *msg;
	sem_t msg_mutex;
};

struct IRC_SRC {
	char *nick;
	char *user;
	char *host;
};

struct IRC_MSG {
	char *_raw;
	struct IRC_SRC *src;
	char *command;
	char **args;
	char *message;
};

extern struct IRC_CTX *ctx;

void free_context();

#endif /* bot.h */
