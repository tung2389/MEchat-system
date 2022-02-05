#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "shared.h"

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

int get_cmd_id(char *cmd) {
    for(int i = 0; i < NUM_CMDS; i++) {
        if(strncmp(commands[i], cmd, strlen(commands[i])) == 0) {
            return i;
        }
    }
    return -1;
}

void execute_command(int sendfd, pollfd *pfds, int fd_count, const char *cmd, int cmd_id, char *nickname) {
    if(strcmp(commands[cmd_id], "/nickname")) {
        int len = strlen(" changes nickname to ");
        char msg[NICKNAME_LEN + len + NICKNAME_LEN + 1]; // +2 to accounts for the distance between the nickname and the actual message
        sprintf(msg, "%s changes nickname to %s", nickname, cmd + strlen("/nickname "));
        send_msg_all(sendfd, pfds, fd_count, msg);
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