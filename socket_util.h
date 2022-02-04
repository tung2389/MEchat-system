#ifndef SOCKET_UTIL
#define SOCKET_UTIL

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include "shared.h"

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr_in6 sockaddr_in6;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct pollfd pollfd;

const char *INVALID_NICKNAME_MSG;
const char *WAIT_MSG;
const char *MATCH_MSG;

int open_clientfd(char *host, char *port);
int open_listenfd(char *port);

void *get_in_addr(sockaddr *sa);
void get_sock_str(sockaddr *sa, char *sock_str);

// Add a new file descriptor to the set
void add_to_pfds(pollfd **pfds, int newfd, int *fd_count, int *fd_size);
// Remove an index from the set
void del_from_pfds(pollfd pfds[], int i, int *fd_count);

#endif