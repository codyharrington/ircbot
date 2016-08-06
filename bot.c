#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "include/bot.h"
#include "include/utils.h"
#include "include/irc.h"
#include "include/io.h"

#define IRC_NICK "chchjesus_bot_two"
#define IRC_SERVER "irc.rizon.net"
#define IRC_CHANNEL "#/g/spam"
#define IRC_PORT 6667


void free_context(struct IRC_CTX *ctx) {
	if (ctx->sockfd && ctx->sockfd >= 0)
		close(ctx->sockfd);
	free_msg(ctx->msg);
	free(ctx);
}

void init_context(struct IRC_CTX *ctx) {
	memset(&(ctx->servaddr), 0, sizeof(ctx->servaddr));
	ctx->servaddr_str = IRC_SERVER;
	ctx->channel = IRC_CHANNEL;
	ctx->port = IRC_PORT;
	ctx->nick = IRC_NICK;
	ctx->msg = (struct IRC_MSG *) calloc(1, sizeof(struct IRC_MSG));
}

void init_connection(struct IRC_CTX *ctx) {
	struct addrinfo hints;
	struct addrinfo *res;
	char port_str[PORT_STR_LEN] = {'\0'};

	memset(&hints, 0, sizeof(hints));
	memset(&res, 0, sizeof(res));
	sprintf(port_str, "%d", ctx->port);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	if (getaddrinfo(ctx->servaddr_str, port_str, &hints, &res) < 0) {
		error("getaddrinfo");
	}
	memcpy(&(ctx->servaddr), res->ai_addr, res->ai_addrlen);
	ctx->servaddr_len = res->ai_addrlen;

	ctx->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (ctx->sockfd < 0) {
		error("socket");
	}
	freeaddrinfo(res);

	if (connect(ctx->sockfd, &(ctx->servaddr), ctx->servaddr_len) < 0) {
		error("connect");
	}
	
	write_to_socket(ctx, "USER %s 0 0 :%s\r\n", ctx->nick, ctx->nick);
	write_to_socket(ctx, "NICK %s\r\n", ctx->nick);
}


int main() {
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);
	int exit_status = 0;
	
	struct IRC_CTX *ctx = (struct IRC_CTX *) malloc(sizeof(struct IRC_CTX));
	
	init_context(ctx);
	init_connection(ctx);
	
	exit_status = listen_server(ctx);
	
	return exit_status;
}
