#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_BUFF_SIZE 1024
#define ADDR_BUFF_SIZE 64
#define STRING_BUFF_SIZE 16
#define IRC_MESSAGE_SIZE 513

struct IRC_CONN {
	struct addrinfo servaddr;
	char *servaddr_str;
	char *channel; 
	char *nick;
	int sockfd;
	int port;
};

void listen_server(struct IRC_CONN *ctx) {
	ssize_t ret;
	char read_buf[IRC_MESSAGE_SIZE];
	connect(ctx->sockfd, ctx->servaddr.ai_addr,
			ctx->servaddr.ai_addrlen);

	while (ret = read(ctx->sockfd, read_buf, IRC_MESSAGE_SIZE - 1)) {
		read_buf[IRC_MESSAGE_SIZE - 1] = '\0';
		puts(read_buf);
	}
}

void resolve_server(struct IRC_CONN *ctx) {
	struct addrinfo hints;
	struct addrinfo *res;
	char port_str[5];

	sprintf(port_str, "%d", ctx->port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ctx->servaddr_str, port_str, &hints, &res);
	ctx->servaddr = *res;
}

void init_context(struct IRC_CONN *ctx) {
	memset(&(ctx->servaddr), 0, sizeof(ctx->servaddr));

	ctx->servaddr_str = "irc.rizon.net\0";
	ctx->channel = "#/g/spam";
	ctx->port = 6667;
	ctx->nick = "chchjesus_bot\0";

	resolve_server(ctx);

	ctx->sockfd = socket(ctx->servaddr.ai_family, 
			ctx->servaddr.ai_socktype,
		   	ctx->servaddr.ai_protocol);
}

/* Signal handler(s) */
static void sigint_handler(int signo) {
	perror("sigint");
	exit(EXIT_FAILURE);
}

static void sigpipe_handler(int signo) {
	perror("sigpipe");
	exit(EXIT_FAILURE);
}

int main()
{
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);
	struct IRC_CONN ctx;

	init_context(&ctx);
	listen_server(&ctx);

	return EXIT_SUCCESS;
}
