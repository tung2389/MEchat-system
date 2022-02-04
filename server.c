#include "socket_util.h"

// User's info that will be passed into new thread
typedef struct usr_info_t {
    int fd;
    char nickname[NICKNAME_LEN + 1];
} usr_info;

bool validNickname(char *nickname);
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
                    int nbytes = recv(pfds[i].fd, nickname, sizeof(nickname) - 1, 0);
                    if(nbytes <= 0) {
                        // This client cannot wait further and disconnected. 
                        if(nbytes == 0) {
                            // If this client has been put in the wait, then remove the client.
                            if(clients != NULL && clients[0].fd == pfds[i].fd) {
                                free(clients);
                                clients = NULL;
                            }
                        }
                        else {
                            fprintf(stderr, "recv error\n");
                        }
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        continue;
                    }

                    if(!validNickname(nickname)) {
                        send(pfds[i].fd, INVALID_NICKNAME_MSG, strlen(INVALID_NICKNAME_MSG), 0);
                        continue;
                    }
                    int clientfd = pfds[i].fd;
                    // Currently there's no client waiting
                    if(clients == NULL) {
                        send(pfds[i].fd, WAIT_MSG, strlen(WAIT_MSG), 0);
                        clients = malloc(2 * sizeof(usr_info));
                        clients[0].fd = clientfd;
                        strcpy(clients[0].nickname, nickname);
                        first_id = i;
                    }
                    // There's already a client waiting. 
                    else {
                        clients[1].fd = clientfd;
                        strcpy(clients[1].nickname, nickname);
                        del_from_pfds(pfds, i, &fd_count);
                        del_from_pfds(pfds, first_id, &fd_count);
                        pthread_create(&tid, NULL, chatHandler, clients);

                        char matched_msg[strlen(MATCH_MSG) + NICKNAME_LEN];
                        strcpy(matched_msg, MATCH_MSG);
                        strcpy(matched_msg + strlen(MATCH_MSG), clients[1].nickname);
                        send(clients[0].fd, matched_msg, strlen(matched_msg), 0);
                        printf(matched_msg);
                        fflush(stdout);

                        strcpy(matched_msg + strlen(MATCH_MSG), clients[0].nickname);
                        send(clients[1].fd, matched_msg, strlen(matched_msg), 0);

                        clients = NULL;
                    }
                }
            }
        }
    }
    return 0;
}

bool validNickname(char *nickname) {
    // Too long or too short
    if(strlen(nickname) == NICKNAME_LEN + 1 || strlen(nickname) <= 1) {
        return false;
    }
    // Contains invalid character
    for(int i = 0; i < strlen(nickname); i++) {
        if(!isalnum(nickname[i]) && nickname[i] != '-' && nickname[i] != '.') {
            return false;
        }
    }
    return true;
}

void *chatHandler(void *arg) {

}
