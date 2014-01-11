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

void send_raw(struct IRC_CONN *ctx, char *fmt, ...) {
    char sbuf[IRC_MESSAGE_SIZE] = {'\0'};
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(sbuf, IRC_MESSAGE_SIZE, fmt, ap);
    va_end(ap);
    printf("<< %s", sbuf);
    write(ctx->sockfd, sbuf, strlen(sbuf));
}

void parse_irc_buffer(char *read_buf) {
    char *msg_end_ptr = NULL;
    int msg_len = 0;

    while ((msg_end_ptr = strstr(read_buf, "\r\n")) != NULL) {
        msg_len = (msg_end_ptr + 1) - read_buf;
        char msg[msg_len];
        memcpy(msg, read_buf, msg_len);
        printf("%s", msg);
        memset(msg, 0, msg_len);
        read_buf += msg_len;
    }
    printf("%s", read_buf);
}

void listen_server(struct IRC_CONN *ctx) {
    ssize_t ret = 0;
    char read_buf[IRC_MESSAGE_SIZE];
    connect(ctx->sockfd, ctx->servaddr.ai_addr,
            ctx->servaddr.ai_addrlen);
    
    send_raw(ctx, "USER %s 0 0 :%s\r\n", ctx->nick, ctx->nick);
    send_raw(ctx, "NICK %s\r\n", ctx->nick);

    for (;;) {
        memset(read_buf, 0, IRC_MESSAGE_SIZE);
        ret = read(ctx->sockfd, read_buf, IRC_MESSAGE_SIZE - 1); 
        switch (ret) {
            case -1:
                close(ctx->sockfd);
                error("Read error");
            case 0:
                success("Finished reading.");
            default:
                parse_irc_buffer(read_buf);
        }
    }
}


void resolve_server(struct IRC_CONN *ctx) {
    struct addrinfo hints;
    struct addrinfo *res;
    char port_str[5] = {'\0'};

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
