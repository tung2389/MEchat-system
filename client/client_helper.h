// If the number of bytes received <= 0, then the server has disconnected, so the client will exit.
ssize_t client_recv(int sockfd, void *buf_raw, size_t len, int flags);

void sendNickname(int clientfd, char *nickname, char *buf);
void handleMatching(int clientfd, char *buf);
void handleChatting(int clientfd, char *buf);