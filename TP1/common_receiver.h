#ifndef COMMON_RECEIVER_H
#define COMMON_RECEIVER_H

#include "common_socket.h"
#define STATIC_BUFFER_SIZE 256

typedef struct receiver{
    char buffer[STATIC_BUFFER_SIZE];
} receiver_t;

int receiver_receive_data(receiver_t *rcv, socket_t *skt, char **request_buff);
void receiver_init(receiver_t *rcv);

#endif
