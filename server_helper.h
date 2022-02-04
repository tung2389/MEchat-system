#ifndef SERVER_HELPER
#define SERVER_HELPER

#include <poll.h>
#include "shared.h"

/*
Functions for intiation phase
*/
bool valid_nickname(char *nickname);


/*
Functions for chatting phase
*/

// Broadcast the message to all clients except the sending client
void send_msg(int sendfd, pollfd *pfds, int fd_count, const char *msg);
bool is_command(char *msg);
bool valid_command(char *cmd);
void execute_command(char *cmd);
bool valid_msg(char *msg);

#endif