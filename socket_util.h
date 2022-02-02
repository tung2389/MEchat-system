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
#include "shared.h"

int open_clientfd(char *host, char *port);
int open_listenfd(char *port);

void *get_in_addr(struct sockaddr *sa);
void get_sock_str(struct sockaddr *sa, char *sock_str);

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size);
// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

#endif