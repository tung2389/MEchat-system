# MEchat system

# Introduction

MEchat contains two applications:

1. A **Multi-threaded, Event-driven** chat server that allows any two random people to chat with each other.
2. A client application that is used to interact with the chat server.

# Requirements

- The server serves as a proxy between two clients. Whenever a client wants to send message to the other, it sends the message to the server, then the server forward that message to the other client.
- The server needs to be able to handle multiple pairs of clients at the same time.
- Any client must be able to send message to the other one at any time, without waiting for the other client to reply.
- Message must be delivered reliably using TCP. Since this is a chat application, it is crucial that messages get delivered without loss or corruption.

# Design

## 1. Communication Protocol:

### Initiation phase

1. Whenever a client wants to chat with another client, it first established the TCP connection with the server. 
2. Once the connection was established (TCP 3-way handshake completed), the client will prompt for an username from the user, and then send that username to the server.
    1. Conditions for the username:
        1. The username can only contain alphanumeric character, hyphen or punctuation mark,
        2. The maximum length of the username is 30 characters.
3. If the username is not valid, then the server send the message “invalid username” to the client, and the client will request another username from the user. 
4. If the username is valid, then the server proceeds to the [matching phase](https://www.notion.so/MEchat-system-18540412dbcf4e89a4b80a20cc29272e).

### Matching phase

1. If all current clients has been matched into pairs, then the server put the client on the wait until there’s a new connected client and send the message “wait” to the client.
2. If there’s another client waiting to be matched, then the server will send the message “matched to {USERNAME}” to both clients to notify that they have been matched to each other. Then, the server proceeds to the [chatting phase](https://www.notion.so/MEchat-system-18540412dbcf4e89a4b80a20cc29272e) and spawn a separate thread to serve the conversation between these two clients.
    1. The reasons that I spawn a new thread to serve each conversation between any two client is that this approach has reasonable complexity. Implementing I/O multiplexing on all clients is quite complex in C, and it involves downloading and building external libraries, such as GLib (for example, you need a hash table to efficiently associate a client with its corresponding partner).

### Chatting phase

1. The server uses I/O multiplexing to detect. The message sent by any client must meet the following