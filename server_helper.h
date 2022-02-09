#ifndef SERVER_HELPER
#define SERVER_HELPER

#include <poll.h>
#include "shared.h"

/*
Functions for intiation phase
*/
// Check if the nickname is valid
bool valid_nickname(char *nickname);


/*
Functions for chatting phase
*/

// Broadcast the message to all clients except the sending client
void send_msg(int sendfd, pollfd *pfds, int fd_count, const char *msg);
// Broadcast the message to all clients
void send_msg_all(int sendfd, pollfd *pfds, int fd_count, const char *msg);
// Check if msg is a command
bool is_command(char *msg);
// Check if the command is valid
bool valid_command(char *cmd);
// Execute the command
void execute_command(int sendfd, pollfd *pfds, int fd_count, const char *cmd, char *nickname, bool *terminate);
// Check if the msg is valid
bool valid_msg(char *msg);

#endif