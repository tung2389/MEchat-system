#ifndef SOCKET_UTIL
#define SOCKET_UTIL

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define MAX_PORT_LEN 5

int open_clientfd(char *host, char *port);
int open_listenfd(char *port);

void *get_in_addr(struct sockaddr *sa);
void get_sock_str(struct sockaddr *sa, char *sock_str);

#endif