#include "socket_util.h"

int main(int argc, char **argv)
{   
    char *host, *port;
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    host = argv[1], port = argv[2];

    int clientfd = open_clientfd(host, port);
    if(clientfd == -1) {
        exit(1);
    }
  
    return 0;
}
