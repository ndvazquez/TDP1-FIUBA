#ifndef COMMON_SOCKET_H
#define COMMON_SOCKET_H

typedef struct socket{
    int fd;
} socket_t;

//Initializes a Socket.
void socket_init(socket_t *skt);
int socket_bind_and_listen(socket_t *skt, char *host, char *port);
int socket_connect(socket_t *skt, char *host, char *port);
//Receives a text based message through a socket.
int socket_receive(socket_t *skt, char *buffer, int size);
//Sends a text based message through a socket.
int socket_send(socket_t *skt, char *buffer, int size);
//Given a passive socket, server, it tries to accept a peer and saves
//the file descriptor in client.
int socket_accept(socket_t *server, socket_t *client);
void socket_shutdown_write(socket_t *skt);
void socket_shutdown_read(socket_t *skt);
//Frees all the resources used by Socket.
void socket_destroy(socket_t *skt);

#endif

