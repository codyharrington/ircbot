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

#define DATA_BUFF_SIZE 1024
#define ADDR_BUFF_SIZE 64
#define STRING_BUFF_SIZE 16
#define IRC_MESSAGE_SIZE 513
#define MSG_QUEUE_LEN 128
#define MSG_TERMINATOR "\r\n"

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

void success(char *msg) {
    printf("%s\n", msg);
    exit(EXIT_SUCCESS);
}

void write_to_socket(struct IRC_CONN *ctx, char *fmt, ...) {
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
        default:
            ;
    }
}

/** 
 * Return the number of bytes left over as 
 * part of a fragmented IRC message
 */
size_t parse_read_buffer(char *read_buf) {
    char *msg_end_ptr = NULL;
    char *read_buf_ptr = read_buf;
    size_t msg_len = 0;

    while ((msg_end_ptr = strstr(read_buf, MSG_TERMINATOR)) != NULL) {
        msg_len = (msg_end_ptr + 2) - read_buf;
        char msg[msg_len + 1];
        memcpy(msg, read_buf, msg_len);
        msg[msg_len] = '\0';

        printf("%s", msg);
        memset(msg, 0, msg_len);
        read_buf += msg_len;
    }
    msg_len = strlen(read_buf);
    memmove(read_buf_ptr, read_buf, msg_len);
    return msg_len;
}

void listen_server(struct IRC_CONN *ctx) {
    ssize_t ret = 0;
    size_t remainder = 0;
    char read_buf[IRC_MESSAGE_SIZE];
    connect(ctx->sockfd, ctx->servaddr.ai_addr,
            ctx->servaddr.ai_addrlen);
    
    write_to_socket(ctx, "USER %s 0 0 :%s\r\n", ctx->nick, ctx->nick);
    write_to_socket(ctx, "NICK %s\r\n", ctx->nick);

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
                remainder = parse_read_buffer(read_buf);
        }
    }
}

void resolve_server(struct IRC_CONN *ctx) {
    struct addrinfo hints;
    struct addrinfo *res;
    char port_str[5] = {'\0'};

    sprintf(port_str, "%d", ctx->port);

    memset(&hints, 0, sizeof(hints));
    memset(&res, 0, sizeof(res));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    /* Need to use freeaddrinfo() here to free the memory */
    getaddrinfo(ctx->servaddr_str, port_str, &hints, &res);
    ctx->servaddr = *res;
}

void init_context(struct IRC_CONN *ctx) {
    memset(&(ctx->servaddr), 0, sizeof(ctx->servaddr));

    ctx->servaddr_str = "irc.rizon.net\0";
    ctx->channel = "#/g/spam";
    ctx->port = 6667;
    ctx->nick = "chchjesus_bot_two\0";

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
