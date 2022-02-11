#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
#include "../socket_util.h"
#include "../util.h"
#include "../shared.h"

ssize_t client_recv(int sockfd, void *buf_raw, size_t len, int flags) {
    char *buf = buf_raw;
    int nbytes = recv_w(sockfd, buf, len, 0);
    if(nbytes <= 0) {
        printf("The server has closed connection.\n");
        exit(0);
    }
    return nbytes;
}

void sendNickname(int clientfd, char *nickname, char *buf) {
    strcpy(buf, INVALID_NICKNAME_MSG);
    while(strcmp(buf, INVALID_NICKNAME_MSG) == 0) {
        printf("Enter your nickname:\n");
        fgets_str(nickname, NICKNAME_LEN, stdin);
        send(clientfd, nickname, NICKNAME_LEN, 0);
        client_recv(clientfd, buf, MSG_LEN, 0);
        if(strcmp(buf, INVALID_NICKNAME_MSG) == 0) {
            printf("Your nickname is invalid.\n");
        }
    }
}

void handleMatching(int clientfd, char *buf) {
    if(strcmp(buf, WAIT_MSG) == 0) {
        printf("Please wait while we find a match for you.\n");
        client_recv(clientfd, buf, MSG_LEN, 0);
    }
    int offset = strlen("#matched_to_");
    char partner_nickname[NICKNAME_LEN];
    strcpy(partner_nickname, buf + offset);
    printf("You get matched to %s. Now both of you can send message to each other.\n", partner_nickname);
}

void handleChatting(int clientfd, char *buf) {
    int fd_count = 2;
    int fd_size = 2;
    pollfd *pfds = malloc(sizeof(pollfd) * fd_size);

    pfds[0].fd = clientfd;
    pfds[1].fd = STDIN_FILENO;

    for(int i = 0; i < fd_count; i++) {
        pfds[i].events = POLLIN;
    }

    while(1) {
        int poll_count = poll(pfds, fd_count, -1);
        for(int i = 0; i < fd_count; i++) {
            if(pfds[i].revents & POLLIN) {
                if(pfds[i].fd == STDIN_FILENO) {
                    fgets_str(buf, MSG_LEN, stdin);
                    send(clientfd, buf, strlen(buf), 0);
                }
                else {
                    int nbytes = client_recv(clientfd, buf, CLIENT_BUF_LEN, 0);
                    if(nbytes <= 0) {
                        printf("The server has closed connection.\n");
                        exit(0);
                    }
                    if(strcmp(buf, INVALID_CMD_MSG) == 0) {
                        printf("Your command is invalid.\n");
                    }
                    else if(strcmp(buf, INVALID_MSG_MSG) == 0) {
                        printf("Your message is invalid.\n");
                    }
                    else if(strcmp(buf, QUIT_MSG) == 0) {
                        printf("Other user has quit the chat. The conversation will be closed now.");
                        exit(0);
                    }
                    // Message from other user.
                    else {
                        printf("\n%s\n", buf);
                    } 
                }
            }
        }
    }
    free(pfds);
}