#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include "bot.h"
#include "irc.h"
#include "utils.h"
#include "io.h"



void display_message(struct IRC_CTX *ctx) {
// 	fprintf(stdout, "%s %s", ctx->msg->src->host, ctx->msg->text);
    debug_print_msg(ctx->msg);
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

int listen_server(struct IRC_CTX *ctx) {
	ssize_t ret = 0;
	size_t offset = 0;
	char read_buf[READ_BUF_SIZE_BYTES];
	int return_value = 0;
	
	for (;;) {
		memset(read_buf + offset, 0, READ_BUF_SIZE_BYTES - offset);
		ret = read(ctx->sockfd, read_buf + offset, 
				(READ_BUF_SIZE_BYTES - 1) - offset); 
		switch (ret) {
			case -1:
				close(ctx->sockfd);
				perror("Read error");
				goto cleanup;
				break;
			case 0:
				printf("Finished reading.");
				goto cleanup;
				break;
			default:
				parse_read_buffer(ctx, read_buf, &offset);
				handle_irc_command(ctx);
				display_message(ctx);
				break;
		}
	}
	
cleanup:
	free_context(ctx);
	return_value =  ret ? errno : ret;
	return return_value;
}