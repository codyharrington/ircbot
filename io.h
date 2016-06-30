#ifndef IO_H
#define IO_H

void display_message(struct IRC_CTX *ctx);

void write_to_socket(struct IRC_CTX *ctx, char *fmt, ...); 

int listen_server(struct IRC_CTX *ctx);

#endif /* io.h */