#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define DATA_BUFF_SIZE 1024
#define ADDR_BUFF_SIZE 64
#define STRING_BUFF_SIZE 16
#define IRC_MESSAGE_SIZE 513
#define MSG_QUEUE_LEN 128

struct IRC_CONN {
	struct addrinfo servaddr;
	char *servaddr_str;
	char *channel; 
	char *nick;
	int sockfd;
	int port;
};

struct IRC_MSG {
	char *msg_type;
	char *msg_body;
	int msg_len;
};

void error(char *msg) {
	perror(msg);
	exit(errno);
}


void parse_buffer(char *buffer) {
	int index = 0;	
	char curr = buffer[index];
	int msg_count = 0;
	int msg_len = 1;

	while (curr != '\0') {
		if (curr == '\n') {
			if (index > 0 && buffer[index - 1] == '\r') {
				char msg_str[index + 1];
                memset(msg_str, 0, index + 1);
				strncpy(msg_str, buffer, msg_len);
				msg_count++;
				msg_len = 0;
                puts(msg_str);
			}
        }
        index++;
        msg_len++;
        curr = buffer[index];
	}
}

void listen_server(struct IRC_CONN *ctx) {
	ssize_t ret;
	char read_buf[IRC_MESSAGE_SIZE];
	connect(ctx->sockfd, ctx->servaddr.ai_addr,
			ctx->servaddr.ai_addrlen);

	for (;;) {
		ret = read(ctx->sockfd, read_buf, IRC_MESSAGE_SIZE - 1); 
		switch (ret) {
			case -1:
				error("Read error");
                break;
			case 0:
				printf("Finished reading");
				exit(0);
                break;
			default:
				read_buf[ret - 1] = '\0';
				parse_buffer(read_buf);
                break;
		}
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
	error("sigint");
}

static void sigpipe_handler(int signo) {
	error("sigpipe");
}

int main() {
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);
	struct IRC_CONN ctx;

	init_context(&ctx);
	listen_server(&ctx);

	return EXIT_SUCCESS;
}
