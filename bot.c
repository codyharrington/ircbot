#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "bot.h"
#include "utils.h"

#define IRC_NICK "chchjesus_bot_two"
#define IRC_SERVER "irc.rizon.net"
#define IRC_CHANNEL "#/g/spam"
#define IRC_PORT 6667


void free_context(struct IRC_CTX *ctx) {
	if (ctx->sockfd && ctx->sockfd >= 0)
		close(ctx->sockfd);
	free_message(ctx->msg);
	sem_destroy(&ctx->msg_mutex);
	free(ctx);
}

void free_message(struct IRC_MSG *msg) {   
	free(msg->src);
	free(msg->_raw);
	free(msg->args);
	if (msg->next != NULL) {
		free(msg->next);
		msg->next = NULL;
	}
	free(msg);
}

void write_to_socket(struct IRC_CTX *ctx, char *fmt, ...) {
	ssize_t ret = 0;
	char sbuf[WRITE_BUF_SIZE_BYTES] = {'\0'};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(sbuf, WRITE_BUF_SIZE_BYTES, fmt, ap);
	va_end(ap);
	printf("<< %s", sbuf);
	ret = write(ctx->sockfd, sbuf, strlen(sbuf));
	switch (ret) {
		case -1:
			error("write");
		case 0:
			printf("wrote nothing");
		default: ;
	}
}

struct IRC_SRC *parse_irc_msg_src(char *raw) {
	struct IRC_SRC *src = (struct IRC_SRC *) malloc(sizeof(struct IRC_SRC));
	src->nick = raw;
	while (*raw != '!') 
		raw++;
	*raw = '\0';
	raw++;
	if (*raw == '~')
		raw++;
	src->user = raw;
	while (*raw != '@')
		raw++;
	*raw = '\0';
	src->host = raw + 1;
	return src;
}

void parse_irc_message(struct IRC_MSG *msg) {   
	size_t src_len = 0;
	char *raw_msg_ptr = msg->_raw;
	char *tmp = NULL;
	size_t n_args = 0;
	size_t i = 0;
	
	NULLIFY_SPACES(raw_msg_ptr)
	if (*raw_msg_ptr == IRC_MSG_HEADER_BOUND) {
		tmp = raw_msg_ptr + 1;
		msg->src = parse_irc_msg_src(tmp);
		NULLIFY_SPACES(raw_msg_ptr)
	}
	msg->command = raw_msg_ptr;
	while (*raw_msg_ptr != ' ')
		raw_msg_ptr++;
	NULLIFY_SPACES(raw_msg_ptr)
	
	tmp = raw_msg_ptr;
	while (*raw_msg_ptr && *raw_msg_ptr != ":") {
		n_args++;
		while (*raw_msg_ptr != ' ')
			raw_msg_ptr++;
		NULLIFY_SPACES(raw_msg_ptr);
	}
	msg->args = (char **) malloc(n_args * sizeof(char *));
	while (*tmp != ':') {
		msg->args[i] = tmp;
		i++;
		while (*tmp)
			tmp++;
		while (!*tmp)
			tmp++;
	}
	raw_msg_ptr++;
	msg->message = raw_msg_ptr;
	while (*raw_msg_ptr != ' ')
		raw_msg_ptr++;
	*raw_msg_ptr = '\0';
}

void parse_read_buffer(struct IRC_CTX *ctx, char *read_buf, size_t *remainder) {
	char *msg_end_ptr = NULL;
	char *read_buf_ptr = read_buf;
	struct IRC_MSG *ctx_msg = ctx->msg;
	size_t msg_len = 0;
	
	while ((msg_end_ptr = strstr(read_buf, IRC_MSG_TERMINATOR)) != NULL) {
		msg_len = (msg_end_ptr + sizeof(IRC_MSG_TERMINATOR)) - read_buf_ptr;
		ctx_msg->_raw = calloc(msg_len + 1, sizeof(char));
		memcpy(ctx_msg->_raw, read_buf, msg_len);
		parse_irc_message(ctx_msg);
		sem_wait(&ctx->msg_mutex);
		NEXT_MSG(ctx_msg)
		sem_post(&ctx->msg_mutex);
		read_buf += msg_len;
	}
	*remainder = strlen(read_buf);
	memmove(read_buf_ptr, read_buf, *remainder);
}


void listen_server(void **args) {
	ssize_t ret = 0;
	size_t offset = 0;
	char read_buf[READ_BUF_SIZE_BYTES];
	struct IRC_CTX *ctx = args[0];
	int *return_value = args[1];
	
	for (;;) {
		memset(read_buf + offset, 0, READ_BUF_SIZE_BYTES - offset);
		ret = read(ctx->sockfd, read_buf + offset, 
				(READ_BUF_SIZE_BYTES - 1) - offset); 
		switch (ret) {
			case -1:
				close(ctx->sockfd);
				perror("Read error");
				goto cleanup;
			case 0:
				printf("Finished reading.");
				goto cleanup;
			default:
				parse_read_buffer(ctx, read_buf, &offset);
				break;
		}
	}
	
cleanup:
	free_context(ctx);
	*return_value =  ret ? errno : ret;
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

void init_context(struct IRC_CTX *ctx) {
	ctx = calloc(1, sizeof(struct IRC_CTX));
	memset(&(ctx->servaddr), 0, sizeof(ctx->servaddr));
	ctx->servaddr_str = IRC_SERVER;
	ctx->channel = IRC_CHANNEL;
	ctx->port = IRC_PORT;
	ctx->nick = IRC_NICK;
	ctx->msg = (struct IRC_MSG *) malloc(sizeof(struct IRC_MSG));
	sem_init(&ctx->msg_mutex, 0, 1);
}


int main() {
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, sigpipe_handler);
	pthread_t ctx_listen_thread;
	void *thread_args[2];
	int exit_status = 0;
	
	struct IRC_CTX *ctx = (struct IRC_CTX *) malloc(sizeof(struct IRC_CTX));
	
	init_context(ctx);
	init_connection(ctx);
	
	thread_args[0] = ctx;
	thread_args[1] = &exit_status;
	
	pthread_create(&ctx_listen_thread, NULL, &listen_server, &thread_args);

	return exit_status;
}
