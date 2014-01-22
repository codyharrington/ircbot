#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define IRC_MESSAGE_SIZE 513
#define PORT_STR_LEN 5
#define MSG_QUEUE_LEN 128
#define IRC_MSG_TERMINATOR "\r\n"
#define IRC_MSG_TERMINATOR_LEN 2

#define IRC_NICK "chchjesus_bot_two"
#define IRC_SERVER "irc.rizon.net"
#define IRC_CHANNEL "#/g/spam"
#define IRC_PORT 6667

#define IRC_HEADER 0
#define IRC_COMMAND 1
#define IRC_PARAMETER_OFFSET 2
#define IRC_FIELD_SEP ' '
#define IRC_PAYLOAD_SEP " :"

struct IRC_CTX {
    struct sockaddr servaddr;
    socklen_t servaddr_len;
    char *servaddr_str;
    char *channel; 
    char *nick;
    int sockfd;
    int port;
};

struct IRC_MSG {
    char **message;
    size_t msg_len;
};

struct IRC_CTX *ctx;
struct IRC_MSG *message_queue;

void free_context() {
    if (ctx->sockfd && ctx->sockfd >= 0)
        close(ctx->sockfd);
    free(ctx);
}

void free_message(struct IRC_MSG *msg, size_t msg_len) {   
    for (size_t idx; idx < msg->msg_len; idx++) {
        free(msg->message[idx]);
    }
}

void error(char *msg) {
    perror(msg);
    free_context();
    free(message_queue);
    exit(errno);
}

void success(char *msg) {
    printf("%s\n", msg);
    free_context();
    free(message_queue);
    exit(EXIT_SUCCESS);
}

static void sigint_handler(int signo) {
    error("sigint");
}

static void sigpipe_handler(int signo) {
    error("sigpipe");
}

void write_to_socket(struct IRC_CTX *ctx, char *fmt, ...) {
    ssize_t ret = 0;
    char sbuf[IRC_MESSAGE_SIZE] = {'\0'};
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(sbuf, IRC_MESSAGE_SIZE, fmt, ap);
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

void parse_irc_message(char *msg, size_t msg_len) {   
   printf("%s", msg); 
}

void parse_read_buffer(char *read_buf, size_t *remainder) {
    char *msg_end_ptr = NULL;
    char *read_buf_ptr = read_buf;
    size_t msg_len = 0;

    while ((msg_end_ptr = strstr(read_buf, IRC_MSG_TERMINATOR)) != NULL) {
        msg_len = (msg_end_ptr + IRC_MSG_TERMINATOR_LEN) - read_buf;
        char msg[msg_len + 1];
        memcpy(msg, read_buf, msg_len);
        msg[msg_len] = '\0';
        
        parse_irc_message(msg, msg_len);
        memset(msg, 0, msg_len);
        read_buf += msg_len;
    }
    *remainder = strlen(read_buf);
    memmove(read_buf_ptr, read_buf, *remainder);
}


void listen_server(struct IRC_CTX *ctx) {
    ssize_t ret = 0;
    size_t remainder = 0;
    char read_buf[IRC_MESSAGE_SIZE];
    
    for (;;) {
        memset(read_buf + remainder, 0, IRC_MESSAGE_SIZE - remainder);
        ret = read(ctx->sockfd, read_buf + remainder, 
                (IRC_MESSAGE_SIZE - 1) - remainder); 
        switch (ret) {
            case -1:
                close(ctx->sockfd);
                error("Read error");
                break;
            case 0:
                success("Finished reading.");
                break;
            default:
                parse_read_buffer(read_buf, &remainder);
                break;
        }
    }
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
}

void init_context() {
    ctx = calloc(1, sizeof(struct IRC_CTX));
    memset(&(ctx->servaddr), 0, sizeof(ctx->servaddr));
    ctx->servaddr_str = IRC_SERVER;
    ctx->channel      = IRC_CHANNEL;
    ctx->port         = IRC_PORT;
    ctx->nick         = IRC_NICK;
}

void init_msg_queue() {
    message_queue = calloc(1, sizeof(struct IRC_MSG));
}


int main() {
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, sigpipe_handler);
    init_context();
    init_msg_queue();
    init_connection(ctx);

    write_to_socket(ctx, "USER %s 0 0 :%s\r\n", ctx->nick, ctx->nick);
    write_to_socket(ctx, "NICK %s\r\n", ctx->nick);
    listen_server(ctx);

    return EXIT_SUCCESS;
}
