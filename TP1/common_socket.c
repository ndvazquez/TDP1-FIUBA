#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "common_socket.h"

static int _request_done(char *buffer, int len){
    buffer[len] = '\0';
    char *tp_delimiter = strstr(buffer, "\n");
    char *real_delimiter = strstr(buffer, "\r\n");
    return tp_delimiter != NULL || real_delimiter != NULL;
}

int socket_init(socket_t *skt, char *host, char *port, int passive){
    struct addrinfo hints;
    struct addrinfo *res;
    int s;

    skt->type = passive ? PASSIVE : ACTIVE;
    skt->peerskt_fd = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;
    
    s = getaddrinfo("localhost", port, &hints, &res);
    if (s != 0) { 
        printf("Error in getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(res);
        return 1;
    }

    skt->fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (skt->fd == -1) {
        printf("Error: %s\n", strerror(errno));
        freeaddrinfo(res);
        return 1;
    }

    if (skt->type == PASSIVE){
        int val = 1;
        s = setsockopt(skt->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            shutdown(skt->fd, SHUT_RDWR);
            close(skt->fd);
            freeaddrinfo(res);
            return 1;
        }

        s = bind(skt->fd, res->ai_addr, res->ai_addrlen);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            shutdown(skt->fd, SHUT_RDWR);
            close(skt->fd);
            freeaddrinfo(res);
            return 1; 
        }

        freeaddrinfo(res);

        s = listen(skt->fd, 10);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            shutdown(skt->fd, SHUT_RDWR);
            close(skt->fd);
            return 1;
        }
    } else {
        s = connect(skt->fd, res->ai_addr, res->ai_addrlen);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(skt->fd);
            freeaddrinfo(res);
            return 1;
        }
        freeaddrinfo(res);
    }

    return 0;
}

int socket_receive_msg(socket_t *skt, char *buffer, int size){
    int s = 0;
    int received = 0;
    int is_the_socket_still_valid = 1;
    int skt_fd;

    if (skt->type == PASSIVE){
        skt->peerskt_fd = accept(skt->fd, NULL, NULL);
        skt_fd = skt->peerskt_fd;
    } else{
        skt_fd = skt->fd;
    }

    while (received < size && !_request_done(buffer, received) 
    && is_the_socket_still_valid){
        s = recv(skt_fd, &buffer[received], size-received, MSG_NOSIGNAL);

        if (s == 0){
            is_the_socket_still_valid = 0;
        } else if (s < 0){
            is_the_socket_still_valid = 0;
        } else{
            received += s;
        }
    }
    shutdown(skt_fd, SHUT_RD);
    if (is_the_socket_still_valid) return received;
    return -1;
}

int socket_send_msg(socket_t *skt, char *buffer, int size){
    int s = 0;
    int sent = 0;
    int is_the_socket_still_valid = 1;
    int skt_fd;

    if (skt->type == PASSIVE){
        skt_fd = skt->peerskt_fd;
    } else{
        skt_fd = skt->fd;
    }

    while (sent < size && is_the_socket_still_valid){
        s = send(skt_fd, &buffer[sent], size-sent, MSG_NOSIGNAL);

        if (s == 0){
            is_the_socket_still_valid = 0;
        } else if (s < 0){
            is_the_socket_still_valid = 0;
        } else{
            sent += s;
        }
    }
    shutdown(skt_fd, SHUT_WR);
    if (skt->type == PASSIVE){
        close(skt_fd);
    }
    if (is_the_socket_still_valid) return sent;
    return -1;  
}

void socket_destroy(socket_t *skt){
    if (skt->peerskt_fd){
        shutdown(skt->peerskt_fd, SHUT_RDWR);
        close(skt->peerskt_fd);
    }
    shutdown(skt->fd, SHUT_RDWR);
    close(skt->fd);
}

