#ifndef BOT_H
#define BOT_H

#include <netdb.h>
#include <semaphore.h>

#define PORT_STR_LEN 5


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

extern struct IRC_CTX *ctx;

void free_context();

void init_context();

void init_connection(struct IRC_CTX *ctx);


#endif /* bot.h */
