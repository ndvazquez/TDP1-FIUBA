#include <string.h>
#include <stdlib.h>
#include "common_receiver.h"

int receiver_receive_data(receiver_t *rcv, socket_t *skt, char **request_ptr){
    char *request_buffer = malloc(sizeof(char) * STATIC_BUFFER_SIZE);
    int request_buffer_size = STATIC_BUFFER_SIZE;
    int keep_receiving = 1;
    int total_received = 0;
    int received = 0;
    int ok = 1;
    while (keep_receiving){
        received = socket_receive(skt, rcv->buffer, STATIC_BUFFER_SIZE);
        if (received <= 0){
            keep_receiving = 0;
            ok = total_received;
        }
        if ((total_received + received) > request_buffer_size){
            request_buffer_size += STATIC_BUFFER_SIZE;
            char *new_buffer = realloc(request_buffer, request_buffer_size);
            if (new_buffer){
                request_buffer = new_buffer;
            } else{
                keep_receiving = 0;
                ok = -1;
            }
        }
        if (received > 0){
            memcpy(request_buffer + total_received, rcv->buffer, received);
            total_received += received;
        }
    }
    request_buffer[total_received] = '\0';

    *request_ptr = request_buffer;

    return ok;
}

void receiver_init(receiver_t *rcv){
    memset(rcv->buffer, 0, STATIC_BUFFER_SIZE);
}
