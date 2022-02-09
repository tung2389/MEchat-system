#ifndef SHARED
#define SHARED

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr_in6 sockaddr_in6;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct pollfd pollfd;

#define BACKLOG 10 
#define MAX_PORT_LEN 5
#define NICKNAME_LEN 30
#define MSG_LEN 2000
#define CLIENT_BUF_LEN 2032 // NICKNAME_LEN + 2 + MSG_LEN
#define NUM_CMDS 2

#define NICK_CMD_ID 0
#define QUIT_CMD_ID 1

const char *INVALID_NICKNAME_MSG;
const char *WAIT_MSG;
const char *MATCH_MSG;
const char *INVALID_CMD_MSG;
const char *INVALID_MSG_MSG;
const char *QUIT_MSG;

const char *commands[NUM_CMDS];

#endif