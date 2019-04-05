#ifndef COMMON_SOCKET_H
#define COMMON_SOCKET_H

typedef enum{
	PASSIVE,
	ACTIVE
}socket_type_t;

typedef struct socket{
    int fd;
    int peerskt_fd;
    socket_type_t type;
} socket_t;

//Initializes a Socket, the argument passive determines if the socket
//will be a passive socket (i.e for using it in a server to accept clients)
//or active (i.e for using it in a client to connect with a given server).
int socket_init(socket_t *skt, char *host, char *port, int passive);
//Receives a text based message through a socket.
int socket_receive_msg(socket_t *skt, char *buffer, int size);
//Sends a text based message through a socket.
int socket_send_msg(socket_t *skt, char *buffer, int size);
//Frees all the resources used by Socket.
void socket_destroy(socket_t *skt);

#endif

