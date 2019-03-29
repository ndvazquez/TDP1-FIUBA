#include "common.h"

int _request_done(char *buffer, int len){
    buffer[len] = '\0';
    char *tp_delimiter = strstr(buffer, "\n");
    char *real_delimiter = strstr(buffer, "\r\n");
    return tp_delimiter != NULL || real_delimiter != NULL;
}

int receive_msg(int skt, char *buffer, int size){
    int s = 0;
    int received = 0;
    int is_the_socket_still_valid = 1;

    while (received < size && !_request_done(buffer, received) 
    && is_the_socket_still_valid){
        s = recv(skt, &buffer[received], size-received, MSG_NOSIGNAL);

        if (s == 0){
            is_the_socket_still_valid = 0;
        } else if (s < 0){
            is_the_socket_still_valid = 0;
        } else{
            received += s;
        }
    }
    if (is_the_socket_still_valid) return received;
    return -1;
}

int send_msg(int skt, char *buffer, int size){
    int s = 0;
    int sent = 0;
    int is_the_socket_still_valid = 1;

    while (sent < size && is_the_socket_still_valid){
        s = send(skt, &buffer[sent], size-sent, MSG_NOSIGNAL);

        if (s == 0){
            is_the_socket_still_valid = 0;
        } else if (s < 0){
            is_the_socket_still_valid = 0;
        } else{
            sent += s;
        }
    }
    if (is_the_socket_still_valid) return sent;
    return -1;  
}
