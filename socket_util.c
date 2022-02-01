#include "socket_util.h"

#define BACKLOG 10 

int open_clientfd(char *host, char *port) {
    int clientfd, rv;
    struct addrinfo hints, *servinfo, *p;
    char server_info[INET6_ADDRSTRLEN + MAX_PORT_LEN + 1];

    memset(&hints, 0, sizeof(struct addrinfo));
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
    struct addrinfo hints, *servinfo, *p;

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

void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &( ((struct sockaddr_in *)sa)->sin_addr );
    }
    return &( ((struct sockaddr_in6 *)sa)->sin6_addr );
}

void get_sock_str(struct sockaddr *sa, char *sock_str) {
    // Get IP address
    inet_ntop(sa->sa_family, get_in_addr(sa), sock_str, INET6_ADDRSTRLEN);
    strcat(sock_str, ":");

    // Get the port
    // Casting to sockaddr_in to access sin_port will return the correct port for both IPv4 and IPv6.
    uint16_t port_num = htons(((struct sockaddr_in *) sa)->sin_port);
    char port_str[MAX_PORT_LEN];
    sprintf(port_str, "%d", port_num);

    strcat(sock_str, port_str);
}