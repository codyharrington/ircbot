#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bot.h"
#include "io.h"
#include "utils.h"
#include "irc.h"

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
    char *raw_msg = NULL;
    
    memcpy(msg->_raw, raw, raw_len);
    msg->_raw_len = raw_len;
    
    raw_msg = msg->_raw;
    
    if (*raw_msg == ':') {
	raw_msg++;
	parse_src(msg->src, raw_msg);
    }
    
    parse_command(msg->command, raw_msg);
    
    if (strchr(raw_msg, ' ') != NULL) {
	jump_to_char(raw_msg, ' ');
	parse_params(&(msg->argc), msg->argv, raw_msg);
    }
    
    if (strchr(raw_msg, ':') != NULL) {
        jump_to_char(raw_msg, ':');
        parse_text(msg->text, raw_msg);
    }
    
    if (!msg_is_valid(msg)) {
        free_msg(msg);
        return NULL;
    }
    
    return msg;
    /* TODO: Check message for mandatory fields. If they do not exist,
     * discard it */
    
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

void parse_src(struct IRC_SRC *src, char *ptr) {
    /* If there is no ! indicating the user section, then it is a 
     * server name */
    if (strchr(ptr, '!') == NULL) {
	src->server_name = ptr;
	return;
    } else {
	src->nick = ptr;
	
	jump_to_char(ptr, '!');
	*ptr = '\0';
	ptr++;
	
	src->user = ptr;
    }
    /* The @ character signifies the start of the host section */
    if (strchr(ptr, '@') == NULL) {
	return;
    } else {
	jump_to_char(ptr, '@');
	ptr++;
	src->host = ptr;
    }
    jump_to_char(ptr, ' ');
    *ptr = '\0';
}

void parse_command(char *command, char *ptr) {
    command = ptr;
}

void parse_params(int *argc, char **argv, char *ptr) {
    char *tmp = ptr;
    int idx = 0;
    
    while (*tmp != '\0' && *tmp != ':') {
	if (*tmp == ' ')
	    *argc += 1;
	while (*tmp == ' ')
	    tmp++;
	tmp++;
    };
    
   argv = (char **) calloc (*argc, sizeof(char *));
   
   for (; idx > 0 && idx < *argc; idx++) {
       argv[idx] = ptr;
       jump_to_char(ptr, ' ');
       *ptr = '\0';
       ptr++;
   }
}

void parse_text(char *text, char *ptr) {
  text = ptr;
}

// void parse_msg(struct IRC_MSG *msg) {   
// 	char *raw_msg_ptr = msg->_raw;
// 	char *tmp = NULL;
// 	size_t n_args = 0;
// 	size_t i = 0;
// 	
// 	nullify_spaces(raw_msg_ptr);
// 	if (*raw_msg_ptr == ':') {
// 		tmp = raw_msg_ptr + 1;
// 		jump_to_space(raw_msg_ptr);
// 		nullify_spaces(raw_msg_ptr);
// 		msg->src = parse_irc_msg_src(tmp);
// 	}
// 	msg->command = raw_msg_ptr;
// 	
// 	jump_to_space(raw_msg_ptr);
// 	nullify_spaces(raw_msg_ptr);
// 	
// 	tmp = raw_msg_ptr;
// 	while (*raw_msg_ptr && *raw_msg_ptr != ':') {
// 		n_args++;
// 		while (*raw_msg_ptr != ' ') {
// 			if (*raw_msg_ptr == ':')
// 				break;
// 			raw_msg_ptr++;
// 		}
// 		nullify_spaces(raw_msg_ptr);
// 	}
// 	msg->argv = (char **) malloc(n_args * sizeof(char *));
// 	while (*tmp != ':') {
// 		msg->argv[i] = tmp;
// 		i++;
// 		while (*tmp)
// 			tmp++;
// 		while (!*tmp)
// 			tmp++;
// 	}
// 	raw_msg_ptr++;
// 	msg->message = raw_msg_ptr;
// }

void parse_read_buffer(struct IRC_CTX *ctx, char *read_buf, size_t *remainder) {
	char *msg_end_ptr = NULL;
	char *read_buf_ptr = read_buf;
	size_t msg_len = 0;
	
	while ((msg_end_ptr = strstr(read_buf, IRC_MSG_TERMINATOR)) != NULL) {
		msg_len = (msg_end_ptr + sizeof(IRC_MSG_TERMINATOR)) - read_buf;
		ctx->msg = create_message(read_buf, msg_len);
		read_buf += msg_len;
	}
	*remainder = strlen(read_buf);
	memmove(read_buf_ptr, read_buf, *remainder);
}