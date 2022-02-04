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

bool is_command(char *msg) {
    if(strlen(msg) > 0 && msg[0] == '/') return true;
    return false;
}

bool valid_command(char *cmd) {
    char *commands[NUM_CMDS] = {"/nickname", "/quit"};
    for(int i = 0; i < NUM_CMDS; i++) {
        if(strcmp(commands[i], cmd) == 0) {
            return true;
        }
    }
    return false;
}

// void execute_command(char *cmd) {
//     if(strcmp(cmd, "/nickname"))
// }

bool valid_msg(char *msg) {
    // Too long or too short
    if(strlen(msg) == MSG_LEN + 1 || strlen(msg) <= 1) {
        return false;
    }
    if(msg[0] == '#') return false;
    return true;
}