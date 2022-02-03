#include "socket_util.h"

int main(int argc, char **argv)
{   
    char *host, *port;
    char nickname[NICKNAME_LEN + 1]; // + 1 to account for the newline character.
    char msg[MSG_LEN];
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

    strcpy(msg, INVALID_NICKNAME_MSG);
    while(strcmp(msg, INVALID_NICKNAME_MSG) == 0) {
        printf("Enter your nickname:\n");
        fgets(nickname, NICKNAME_LEN + 1, stdin);
        // Delete the newline character
        nickname[strlen(nickname) - 1] = '\0';
        send(clientfd, nickname, NICKNAME_LEN, 0);
        recv(clientfd, msg, sizeof msg, 0);
        if(strcmp(msg, INVALID_NICKNAME_MSG) == 0) {
            printf("Your nickname is invalid\n");
        }
    }

    close(clientfd);
    return 0;
}
