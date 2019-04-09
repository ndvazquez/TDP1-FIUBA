#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "common_socket.h"

static int _socket_create(socket_t *skt, int ai_family,
                int ai_socktype, int ai_protocol){
    skt->fd = socket(ai_family, ai_socktype, ai_protocol);
    if (skt->fd == -1) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

static int _wrapper_getaddrinfo(char *host, char *port, struct addrinfo **ptr,
            int passive){
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;

    int s = getaddrinfo(host, port, &hints, &res);
    if (s != 0) { 
        printf("Error in getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(*ptr);
        return -1;
    }
    *ptr = res;
    return 0;
}

int socket_bind_and_listen(socket_t *skt, char *host, char *port){
    struct addrinfo *res = NULL;
    if (_wrapper_getaddrinfo(host, port, &res, 1)){
        return -1;
    }
    int did_we_managed_to_bind = 0;
    struct addrinfo *ptr;
    for (ptr = res; ptr && !did_we_managed_to_bind; ptr = ptr->ai_next){
        if (_socket_create(skt, ptr->ai_family,
                ptr->ai_socktype, ptr->ai_protocol)){
            continue;
        }
        int val = 1;
        int s = setsockopt(skt->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(skt->fd);
            continue;
        }

        s = bind(skt->fd, ptr->ai_addr, ptr->ai_addrlen);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(skt->fd);
            continue; 
        }

        s = listen(skt->fd, 10);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(skt->fd);
            continue;
        }
        did_we_managed_to_bind = (s != -1);
    }
    freeaddrinfo(res);
    return did_we_managed_to_bind ? 0 : -1;
}

int socket_connect(socket_t *skt, char *host, char *port){
    struct addrinfo *res = NULL;
    if (_wrapper_getaddrinfo(host, port, &res, 0)){
        return -1;
    }

    struct addrinfo *ptr;
    int are_we_connected = 0;
    for (ptr = res; ptr && !are_we_connected; ptr = ptr->ai_next){
        if (_socket_create(skt, ptr->ai_family,
                ptr->ai_socktype, ptr->ai_protocol)){
            continue;
        }
        int s = connect(skt->fd, res->ai_addr, res->ai_addrlen);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(skt->fd);
            continue;
        }
        are_we_connected = (s != -1);
    }
    freeaddrinfo(res);
    return are_we_connected ? 0 : -1;
}

void socket_init(socket_t *skt){
    skt->fd = -1;
}

int socket_receive(socket_t *skt, char *buffer, int size){
    int s = 0;
    int received = 0;
    int is_the_socket_still_valid = 1;

    while (received < size && is_the_socket_still_valid){
        s = recv(skt->fd, &buffer[received], size-received, MSG_NOSIGNAL);
        if (s == 0){
            if (received == 0) is_the_socket_still_valid = 0;
            break;
        } else if (s < 0){
            is_the_socket_still_valid = 0;
        } else{
            received += s;
        }
    }
    if (is_the_socket_still_valid) return received;
    return -1;
}

int socket_send(socket_t *skt, char *buffer, int size){
    int s = 0;
    int sent = 0;
    int is_the_socket_still_valid = 1;

    while (sent < size && is_the_socket_still_valid){
        s = send(skt->fd, &buffer[sent], size-sent, MSG_NOSIGNAL);
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

int socket_accept(socket_t *server, socket_t *client){
    client->fd = accept(server->fd, NULL, NULL);
    if (client->fd == -1){
        printf("Error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void socket_shutdown_write(socket_t *skt){
    shutdown(skt->fd, SHUT_WR);
}

void socket_shutdown_read(socket_t *skt){
    shutdown(skt->fd, SHUT_RD);
}

void socket_destroy(socket_t *skt){
    if (skt->fd != -1){
        shutdown(skt->fd, SHUT_RDWR);
        close(skt->fd);
    }     
}

