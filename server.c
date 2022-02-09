#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

#include "socket_util.h"
#include "util.h"
#include "server_helper.h"
#include "shared.h"

// User's info that will be passed into new thread
typedef struct {
    int fd;
    char *nickname;
} usr_info;

void *chatHandler(void *arg);

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    char *port;
    int listenfd; // Listening socket descriptor
    sockaddr_storage client_addr; // Client address
    socklen_t addrlen;
    char client_info[INET6_ADDRSTRLEN + MAX_PORT_LEN + 2]; // + 1 to account for the newline character.
    char nickname[NICKNAME_LEN + 2]; // + 1 to check for over-size nickname.
    /*
    The index of the first client, who goes into wait list first, in the poll watch list. It is used for removal when this client get matched.
    */
    int first_id; 
    pthread_t tid; 

    port = argv[1];

    // Start off with room for 6 connections and realloc as necessary
    int fd_count = 0;
    int fd_size = 6;
    pollfd *pfds = malloc(sizeof(pollfd) * fd_size);

    listenfd = open_listenfd(port);
    if(listenfd == -1) {
        exit(1);
    }

    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;
    fd_count = 1;

    usr_info *clients = NULL;
    while(1) {
        int poll_count = poll(pfds, fd_count, -1);
        if(poll_count == -1) {
            fprintf(stderr, "poll error\n");
            exit(1);
        }
        
        // Run through the existing connections looking for data to read    
        for(int i = 0; i < fd_count; i++) {
            // Check if someone's ready to read
            if(pfds[i].revents & POLLIN) {
                if(pfds[i].fd == listenfd) { // New connection
                    addrlen = sizeof(client_addr);
                    int connfd = accept(listenfd, (sockaddr *) &client_addr, &addrlen);
                    if(connfd == -1) {
                        fprintf(stderr, "accept error\n");
                        continue;
                    }
                    add_to_pfds(&pfds, connfd, &fd_count, &fd_size);

                    get_sock_str((sockaddr *) &client_addr, client_info);
                    printf("server: got connection from %s\n", client_info);
                }
                /*
                Some client sends a nickname. Remove the client from pfds, 
                and either put the client on the wait, or match with another client, depending on the situation
                */
                else {
                    int nbytes = recv_w(pfds[i].fd, nickname, sizeof(nickname) - 1, 0);
                    // This client cannot wait further and disconnected. 
                    if(nbytes <= 0) {
                        if(nbytes < 0) {
                            fprintf(stderr, "recv error\n");
                        }
                        // If this client has been put in the wait, then remove the client.
                        if(clients != NULL && clients[0].fd == pfds[i].fd) {
                            free(clients);
                            clients = NULL;
                        }
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        continue;
                    }

                    if(!valid_nickname(nickname)) {
                        send(pfds[i].fd, INVALID_NICKNAME_MSG, strlen(INVALID_NICKNAME_MSG), 0);
                        continue;
                    }
                    int clientfd = pfds[i].fd;
                    // Currently there's no client waiting
                    if(clients == NULL) {
                        send(pfds[i].fd, WAIT_MSG, strlen(WAIT_MSG), 0);
                        clients = malloc(2 * sizeof(usr_info));
                        clients[0].fd = clientfd;
                        clients[0].nickname = malloc_str(strlen(nickname));
                        strcpy(clients[0].nickname, nickname);
                        first_id = i;
                    }
                    // There's already a client waiting. 
                    else {
                        clients[1].fd = clientfd;
                        clients[1].nickname = malloc_str(strlen(nickname));
                        strcpy(clients[1].nickname, nickname);
                        del_from_pfds(pfds, i, &fd_count);
                        del_from_pfds(pfds, first_id, &fd_count);

                        char matched_msg[strlen(MATCH_MSG) + NICKNAME_LEN];
                        sprintf(matched_msg, "%s%s", MATCH_MSG, clients[1].nickname);
                        send(clients[0].fd, matched_msg, strlen(matched_msg), 0);

                        sprintf(matched_msg, "%s%s", MATCH_MSG, clients[0].nickname);
                        send(clients[1].fd, matched_msg, strlen(matched_msg), 0);

                        pthread_create(&tid, NULL, chatHandler, clients);
                        clients = NULL;
                    }
                }
            }
        }
    }

    free(pfds);
    return 0;
}

void *chatHandler(void *arg_raw) {
    pthread_detach(pthread_self());
    usr_info *arg = (usr_info *) arg_raw;
    char msg[MSG_LEN + 2]; // +1 to check for message which is too long

    int fd_count = 2;
    int fd_size = 2;
    pollfd *pfds = malloc(sizeof(pollfd) * fd_size);
    /*
    Save all nicknames in paralell with their corresponding socket descriptor using an array, since we do not have a hash table in C to keep track of 
    this association. Later, if the need for adding more user or removing users to and from the this chat room arises, the adding and removing function
    for nicknames can be implemented the same as for pfds. 
    */
    char **nicknames = malloc(sizeof(char *) * fd_size);
    for(int i = 0; i < fd_size; i++) {
        pfds[i].fd = arg[i].fd;
        pfds[i].events = POLLIN;
        nicknames[i] = arg[i].nickname;
    }
    free(arg);
    bool terminate = false; // terminate = true means at least one client disconnected, so close the thread.
    while(!terminate) {
        int poll_count = poll(pfds, fd_count, -1);
        if(poll_count == -1) {
            fprintf(stderr, "poll error\n");
            exit(1);
        }
        
        // Run through the existing connections looking for data to read    
        for(int i = 0; i < fd_count; i++) {
            if(terminate) break;
            if(pfds[i].revents & POLLIN) {
                int nbytes = recv_w(pfds[i].fd, msg, sizeof(msg) - 1, 0);
                // This client has disconnected
                if(nbytes <= 0) {
                    if(nbytes < 0) {
                        fprintf(stderr, "recv error\n");
                    }
                    execute_command(pfds[i].fd, pfds, fd_count, "/quit", nicknames[i], &terminate);
                    break;
                }
                if(is_command(msg)) {
                    execute_command(pfds[i].fd, pfds, fd_count, msg, nicknames[i], &terminate);
                }
                else {
                    char buf[NICKNAME_LEN + MSG_LEN + 3]; // +2 to accounts for the distance between the nickname and the actual message
                    if(!valid_msg(msg)) {
                        send(pfds[i].fd, INVALID_MSG_MSG, strlen(INVALID_MSG_MSG), 0);
                        continue;
                    }
                    sprintf(buf, "%s: %s", nicknames[i], msg);
                    send_msg_all(pfds[i].fd, pfds, fd_count, buf);
                }
            }
        }
    }

    free(pfds);
    for(int i = 0; i < fd_count; i++) {
        free(nicknames[i]);
    }
    free(nicknames);
}
