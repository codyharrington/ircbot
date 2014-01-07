#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>

#define DATA_BUFF_SIZE 1024
#define ADDR_BUFF_SIZE 64
#define STRING_BUFF_SIZE 16
#define IRC_MESSAGE_SIZE 512

struct IRC_CONN {
	struct sockaddr_in servaddr;
	char *server_addr_string;
	char *channel; 
	char *nick;
	int sockfd;
	int port;
};

void listen_server(struct IRC_CONN *ctx) {
	/* use fgets(). A message is defined as a string, terminated by
	 * carriage-return (\r) and line-feed(\n)
	 */
	while (1) {
		continue;
	}
}

void init_context(struct IRC_CONN *ctx) {
	memset(&(ctx->servaddr), 0, sizeof(ctx->servaddr));

	ctx->server_addr_string = "irc.rizon.net\0";
	ctx->channel = "#/g/spam";
	ctx->port = 6667;
	ctx->nick = "chchjesus_bot\0";

	ctx->servaddr.sin_family = AF_INET;
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
	struct IRC_CONN *ctx;
	init_context(ctx);
	listen_server(ctx);
	return EXIT_SUCCESS;
}
