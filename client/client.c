#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "client_helper.h"
#include "../socket_util.h"
#include "../util.h"
#include "../shared.h"

int main(int argc, char **argv)
{   
    char *host, *port;
    char nickname[NICKNAME_LEN + 2]; // + 1 to account for the newline character.
    char buf[CLIENT_BUF_LEN + 1];
    // Don't buffer on stdout, since this is a chat app.
    setbuf(stdout, NULL);
    
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    host = argv[1], port = argv[2];

    int clientfd = open_clientfd(host, port);
    if(clientfd == -1) {
        exit(1);
    }
    sendNickname(clientfd, nickname, buf);
    handleMatching(clientfd, buf);
    handleChatting(clientfd, buf);

    close(clientfd);
    return 0;
}