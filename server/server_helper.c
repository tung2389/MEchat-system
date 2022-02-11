#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
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

#include "server_helper.h"
#include "../socket_util.h"
#include "../shared.h"

/*
Functions for intiation phase
*/
bool valid_nickname(char *nickname) {
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


/*
Functions for chatting phase
*/

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

void send_msg(int sendfd, pollfd *pfds, int fd_count, const char *msg) {
    for(int i = 0; i < fd_count; i++) {
        if(pfds[i].fd == sendfd) continue;
        send(pfds[i].fd, msg, strlen(msg), 0);
    }
}

void send_msg_all(int sendfd, pollfd *pfds, int fd_count, const char *msg) {
    for(int i = 0; i < fd_count; i++) {
        send(pfds[i].fd, msg, strlen(msg), 0);
    }
};

bool is_command(char *msg) {
    if(strlen(msg) > 0 && msg[0] == '/') return true;
    return false;
}

int get_cmd_id(const char *cmd) {
    for(int i = 0; i < NUM_CMDS; i++) {
        if(strncmp(commands[i], cmd, strlen(commands[i])) == 0) {
            return i;
        }
    }
    return -1;
}

void execute_command(int sendfd, pollfd *pfds, int fd_count, const char *cmd, char *nickname, bool *terminate) {
    int cmd_id = get_cmd_id(cmd);
    if(cmd_id == -1) {
        send(sendfd, INVALID_CMD_MSG, strlen(INVALID_CMD_MSG), 0);
        return;
    }
    if(cmd_id == NICK_CMD_ID) {
        int len = strlen(" changes nickname to ");
        char msg[NICKNAME_LEN + len + NICKNAME_LEN + 1]; // +2 to accounts for the distance between the nickname and the actual message
        sprintf(msg, "%s changes nickname to %s", nickname, cmd + strlen("/nickname "));
        strcpy(nickname, cmd + strlen("/nickname "));
        send_msg_all(sendfd, pfds, fd_count, msg);
    }
    else if(cmd_id == QUIT_CMD_ID) {
        send_msg(sendfd, pfds, fd_count, QUIT_MSG);
        // Close all connections in this thread
        for(int j = 0; j < fd_count; j++) {
            close(pfds[j].fd);   
        }         
        *terminate = true;
    }
}

bool valid_msg(char *msg) {
    // Too long or too short
    if(strlen(msg) == MSG_LEN + 1 || strlen(msg) <= 1) {
        return false;
    }
    if(msg[0] == '#') return false;
    return true;
}