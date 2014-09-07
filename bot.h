#ifndef BOT_H
#define BOT_H

#include <netdb.h>
#include <semaphore.h>

#define IRC_MESSAGE_SIZE 513
#define PORT_STR_LEN 5
#define IRC_MSG_TERMINATOR "\r\n"
#define READ_BUF_SIZE_BYTES 1024
#define WRITE_BUF_SIZE_BYTES 1024

#define NEXT_MSG(msg) \
	msg->next = (struct IRC_MSG *) malloc(sizeof(struct IRC_MSG)); \
	msg->next->prev = msg; \
	msg->next->next = NULL; \
	if (msg->prev != NULL) { \
		free(msg->prev); \
		msg->prev = NULL; \
	} \
	msg = msg->next; 

#define PREV_MSG(msg) \
	msg = msg->prev;

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
	// Linked list up in here
	struct IRC_MSG *next;
	struct IRC_MSG *prev;
};

struct THREAD_ARGS {
	struct IRC_CTX *ctx;
	int *return_value;
};

extern struct IRC_CTX *ctx;

void free_context();

#endif /* bot.h */
