#ifndef SOCKET_UTIL
#define SOCKET_UTIL

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>

#include "shared.h"

int open_clientfd(char *host, char *port);
int open_listenfd(char *port);

// Return the IP address of sockaddr.
void *get_in_addr(sockaddr *sa);
// Return the address of sockaddr in the format [IP]:[PORT] to the string sock_str.
void get_sock_str(sockaddr *sa, char *sock_str);

// Add a new file descriptor to the set of pollfd
void add_to_pfds(pollfd **pfds, int newfd, int *fd_count, int *fd_size);
// Remove file descriptor at index i of the set of pollfd from the set
void del_from_pfds(pollfd pfds[], int i, int *fd_count);

// recv from sockfd and overwrite buf (by append NULL-terminator at the end of new data)
ssize_t recv_w(int sockfd, void *buf, size_t len, int flags);

#endif