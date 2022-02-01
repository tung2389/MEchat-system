#include "socket_util.h"

int main(int argc, char **argv)
{
    char *port;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    char client_info[INET6_ADDRSTRLEN + MAX_PORT_LEN + 1];

    if(argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    port = argv[1];

    int listenfd = open_listenfd(port);
    if(listenfd == -1) {
        exit(1);
    }

    int newfd = accept(listenfd, (struct sockaddr *) &client_addr, &sin_size);
    if(newfd == -1) {
        fprintf(stderr, "Server failed to accept a connection");
        exit(1);
    }

    get_sock_str((struct sockaddr *) &client_addr, client_info);
    printf("server: got connection from %s\n", client_info);

    close(newfd);
    return 0;
}
