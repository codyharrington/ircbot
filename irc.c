#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/bot.h"
#include "include/io.h"
#include "include/utils.h"
#include "include/irc.h"

struct IRC_MSG *init_msg()  {
    struct IRC_MSG *msg = (struct IRC_MSG *) calloc(1, sizeof(struct IRC_MSG));
    msg->_raw = NULL;
    msg->_raw_len = 0;
    
    msg->src = (struct IRC_SRC *) calloc(1, sizeof(struct IRC_SRC));
    msg->src->server_name = NULL;
    msg->src->nick = NULL;
    msg->src->user = NULL;
    msg->src->host = NULL;
    
    msg->command = NULL;
    msg->argc = 0;
    msg->argv = NULL;
    msg->text = NULL;
    msg->valid = 0;
    return msg;
}

void free_msg(struct IRC_MSG *msg) {
	free(msg->src);
	free(msg->argv);
	free(msg->_raw);
	free(msg);
}

/* Taken from RFC spec https://tools.ietf.org/html/rfc1459#section-2.3.1 */
struct IRC_MSG *create_message(char *raw, size_t raw_len) {
    struct IRC_MSG *msg = init_msg();
    msg->_raw = calloc(raw_len + 1, sizeof(char));
    char *raw_msg_ptr = NULL;
    char *delim_match = NULL;
    
    memcpy(msg->_raw, raw, raw_len);
    msg->_raw_len = raw_len;
    
    raw_msg_ptr = msg->_raw;
    
    delim_match = strpbrk(raw_msg_ptr, ": ");
    if (delim_match != NULL && *delim_match == ':') {
	raw_msg_ptr = parse_src(msg, ++raw_msg_ptr);
    }

    /* According to the IRC spec, command is the only mandatory
     * section */
    delim_match = strpbrk(raw_msg_ptr, " :");
    if (delim_match != NULL && *delim_match == ' ') {
        msg->command = strsep(&raw_msg_ptr, " ");
    }
    delim_match = strpbrk(raw_msg_ptr, " :");
    if (delim_match != NULL && *delim_match == ' ') {
	raw_msg_ptr = parse_params(msg, raw_msg_ptr);
    }
    delim_match = strpbrk(raw_msg_ptr, ":\r\n");
    if (delim_match != NULL && *delim_match == ':') {
       raw_msg_ptr = parse_text(msg, ++raw_msg_ptr);
    }
    
    /* If the message is deemed invalid, we will discard it later */
    msg->valid = msg_is_valid(msg);
    
    return msg;
    
}

int msg_is_valid(struct IRC_MSG *msg) {
    /* Check command */
    if (msg->command == NULL)
      return 0;
    
    return 1;
}

void handle_irc_command(struct IRC_CTX *ctx) {
	if (!strcmp(ctx->msg->command, "PING"))
		write_to_socket(ctx, "PONG :%s\r\n", ctx->msg->src->host);
}

char *parse_src(struct IRC_MSG *msg, char *raw_msg_ptr) {
    if (*strpbrk(raw_msg_ptr, "! ") != '!') {
	msg->src->server_name = strsep(&raw_msg_ptr, " ");
	return raw_msg_ptr;
    } else {
	msg->src->nick = strsep(&raw_msg_ptr, "!");
    }
    /* The @ character signifies the start of the host section */
    switch (*strpbrk(raw_msg_ptr, "@ ")) {
	case '@':
	    msg->src->user = strsep(&raw_msg_ptr, "@");
	    msg->src->host = strsep(&raw_msg_ptr, " ");
	    break;
	case ' ':
	    msg->src->user = strsep(&raw_msg_ptr, " ");
	    break;
    }
    return raw_msg_ptr;
}

char *parse_command(struct IRC_MSG *msg, char *raw_msg_ptr) { 
    msg->command = strsep(&raw_msg_ptr, " ");
    return raw_msg_ptr;
}

char *parse_params(struct IRC_MSG *msg, char *raw_msg_ptr) {
    char **argv = (char **) calloc (MAX_PARAM_LIMIT, sizeof(char *));
    
    while (*raw_msg_ptr != ':' && msg->argc < MAX_PARAM_LIMIT) {
	argv[msg->argc] = strsep(&raw_msg_ptr, " ");
	msg->argc += 1;
    }
    msg->argv = argv;
    
    return raw_msg_ptr;
}

char *parse_text(struct IRC_MSG *msg, char *raw_msg_ptr) {
    msg->text = strsep(&raw_msg_ptr, "\r\n");
    return raw_msg_ptr;
}

void parse_read_buffer(struct IRC_CTX *ctx, char *read_buf, size_t *remainder_size) {
	char *msg_end_ptr = NULL;
	char *read_buf_ptr = read_buf;
	size_t msg_len = 0;
	
	while ((msg_end_ptr = strstr(read_buf, IRC_MSG_TERMINATOR)) != NULL) {
		msg_len = (msg_end_ptr + sizeof(IRC_MSG_TERMINATOR)) - read_buf; 
		ctx->msg = create_message(read_buf, msg_len);
		 /* We subtract 1 because msg_len is the total length, and 
		  * indexes are 0-based */
		read_buf += (msg_len - 1); 
	}
	*remainder_size = strlen(read_buf);
	memmove(read_buf_ptr, read_buf, *remainder_size);
}
