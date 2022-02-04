#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>

#include "shared.h"

void *get_in_addr(sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &( ((sockaddr_in *)sa)->sin_addr );
    }
    return &( ((sockaddr_in6 *)sa)->sin6_addr );
}

void get_sock_str(sockaddr *sa, char *sock_str) {
    // Get IP address
    inet_ntop(sa->sa_family, get_in_addr(sa), sock_str, INET6_ADDRSTRLEN);
    strcat(sock_str, ":");

    // Get the port
    // Casting to sockaddr_in to access sin_port will return the correct port for both IPv4 and IPv6.
    uint16_t port_num = htons(((sockaddr_in *) sa)->sin_port);
    char port_str[MAX_PORT_LEN];
    sprintf(port_str, "%d", port_num);

    strcat(sock_str, port_str);
}

int open_clientfd(char *host, char *port) {
    int clientfd, rv;
    addrinfo hints, *servinfo, *p;
    char server_info[INET6_ADDRSTRLEN + MAX_PORT_LEN + 2]; // including the ':' character

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if(rv = getaddrinfo(host, port, &hints, &servinfo) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        if(connect(clientfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(clientfd);
            continue;
        }
        break;
    }

    if(p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        freeaddrinfo(servinfo);
        return -1;
    }

    get_sock_str(p->ai_addr, server_info);
    printf("client: connected to %s\n", server_info);

    freeaddrinfo(servinfo);
    return clientfd;
}

int open_listenfd(char *port) {
    int listenfd, rv;
    int yes = 1; 
    addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            fprintf(stderr, "setsockopt failed");
            return -1;
        }

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(listenfd);
            continue;
        }
        break;
    }
    
    freeaddrinfo(servinfo);

    if(p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return -1;
    }

    if (listen(listenfd, BACKLOG) == -1) {
        fprintf(stderr, "server: failed to listen\n");
        return -1;
    }

    return listenfd;
} 

void add_to_pfds(pollfd **pfds, int newfd, int *fd_count, int *fd_size) {
    if(*fd_count == *fd_size) {
        *fd_size *= 2; // Double the size of the array
        *pfds = realloc(*pfds, sizeof(pollfd) * (*fd_size));
    }
    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

void del_from_pfds(pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}