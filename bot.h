#ifndef BOT_H
#define BOT_H

#include <netdb.h>

#define IRC_MESSAGE_SIZE 513
#define PORT_STR_LEN 5
#define MSG_QUEUE_LEN 128
#define IRC_MSG_TERMINATOR "\r\n"
#define IRC_MSG_TERMINATOR_LEN 2

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

extern struct IRC_CTX *ctx;
extern struct IRC_MSG *message_queue;


#endif /* bot.h */
