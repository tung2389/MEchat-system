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
2. Once the connection was established (TCP 3-way handshake completed), the client will prompt for an nickname from the user, and then send that nickname to the server.
    1. Conditions for the nickname:
        1. The nickname can only contain alphanumeric character, hyphen or punctuation mark,
        2. The maximum length of the nickname is 30 characters.
        3. The nickname must contain at least one character.
3. If the nickname is not valid, then the server send the message "#invalid_nickname" to the client, and the client will request another nickname from the user. 
4. If the nickname is valid, then the server proceeds to the [matching phase](#Matching-phase).

### Matching phase

1. If all current clients has been matched into pairs, then the server put the client on the wait until there’s a new connected client and send the message "#wait" to the client.
2. If there’s another client waiting to be matched, then the server will send the message "#matched_to_[NICKNAME]” to both clients to notify that they have been matched to each other. Then, the server proceeds to the [chatting phase](#Chatting-phase) and spawn a separate thread to serve the conversation between these two clients.
    1. The reasons that I spawn a new thread to serve each conversation between any two client is that this approach has reasonable complexity. Implementing I/O multiplexing on all clients is quite complex in C, and it involves downloading and building external libraries, such as GLib (for example, you need a hash table to efficiently associate a client with its corresponding partner).

### Chatting phase

1. The server uses I/O multiplexing to catch messages sent by clients. The message sent by any client must meet the following condition:
    1. The size of the message cannot be bigger than 2,000 bytes.
    2. The message must contain at least one character.
    3. The message cannot start with the character '#'. Strings that start with the '#' symbol are reserved for notification messages that the server sends to clients.
2. If the message starts with the character '/', it is interpreted as a command. Here is the link to [list of commands](#2-Commands). 
    1. If the command is valid, then the server execute the command. 
    2. If the command is invalid, then the server send the message "#invalid_command" to the sending client. 
3. If the message is invalid, then the server send the message "#invalid_message" to the sending client.
4. If the message is valid, then the server forwards the mesage to the other client.
5. If one client close its connections, either by sending the command ```/quit``` or when user hitting ```Ctrl-D``` on the terminal,the server proceeds to the [ending phase](#Ending-phase)

### Ending phase

1. If one client close its connections, the server will send the message "#user_quit" to the other client to notify it. 
2. Next, the server will close sockets associated with these two clients, thus closing connections to these clients, and close the thread of this conversation.
3. The other client, after received the message "#user_quit", will also close its socket.

## 2. Commands
| Command       | Parameter             | Description                         |
| ------------- | --------------------- | ----------------------------------- |
| /quit         |                       | Quit the chat, close the connection |
| /nickname     | [nickname]            | Change nickname                     |


- For I/O multiplexing, I will use ```poll``` instead of ```epoll``` for the sake of portability, although ```epoll``` scales better. ```poll``` is a POSIX standard interface, while epoll is Linux-specific.
- At the client side, the user sends a message by entering the message in the terminal, then hit ```Enter``` key.
- When receive a new client without any waiting client, the server will pass