#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "socket.h"

int Socket::_wrapperGetAddrinfo(std::string host, std::string port,
                                struct addrinfo **ptr,
                                int passive){
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;

    int s = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (s != 0) { 
        printf("Error in getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(*ptr);
        return -1;
    }
    *ptr = res;
    return 0;
}

int Socket::_socketCreate(int ai_family, int ai_socktype, int ai_protocol){
    int fd = socket(ai_family, ai_socktype, ai_protocol);
    return fd;
}

Socket::Socket(){
    this->_fd = -1;
}

Socket::Socket(int fileDescriptor) : _fd(fileDescriptor){}

Socket::~Socket(){
    if (this->_fd != -1){
        shutdown(this->_fd, SHUT_RDWR);
        close(this->_fd);
    } 
}

int Socket::bindAndListen(std::string host, std::string port){
    struct addrinfo *res = NULL;
    if (_wrapperGetAddrinfo(host, port, &res, 1)){
        return -1;
    }
    int did_we_managed_to_bind = 0;
    struct addrinfo *ptr;
    for (ptr = res; ptr && !did_we_managed_to_bind; ptr = ptr->ai_next){
        this->_fd =_socketCreate(ptr->ai_family, ptr->ai_socktype,
         ptr->ai_protocol);
        if (this->_fd == -1){
            continue;
        }
        int v = 1;
        int s = setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(this->_fd);
            continue;
        }

        s = bind(this->_fd, ptr->ai_addr, ptr->ai_addrlen);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(this->_fd);
            continue; 
        }

        s = listen(this->_fd, 10);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(this->_fd);
            continue;
        }
        did_we_managed_to_bind = (s != -1);
    }
    freeaddrinfo(res);
    return did_we_managed_to_bind ? 0 : -1;
}

int Socket::connectToHost(std::string host, std::string port){
    struct addrinfo *res = NULL;
    if (_wrapperGetAddrinfo(host, port, &res, 0)){
        return -1;
    }

    struct addrinfo *ptr;
    int are_we_connected = 0;
    for (ptr = res; ptr && !are_we_connected; ptr = ptr->ai_next){
        this->_fd =_socketCreate(ptr->ai_family, ptr->ai_socktype,
         ptr->ai_protocol);
        if (this->_fd == -1){
            continue;
        }
        int s = connect(this->_fd, res->ai_addr, res->ai_addrlen);
        if (s == -1) {
            printf("Error: %s\n", strerror(errno));
            close(this->_fd);
            continue;
        }
        are_we_connected = (s != -1);
    }
    freeaddrinfo(res);
    return are_we_connected ? 0 : -1;
}

int Socket::receiveMessage(char *buffer, int size){
    int s = 0;
    int received = 0;
    int is_the_socket_still_valid = 1;

    while (received < size && is_the_socket_still_valid){
        s = recv(this->_fd, &buffer[received], size-received, MSG_NOSIGNAL);
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

int Socket::sendMessage(char *buffer, int size){
    int s = 0;
    int sent = 0;
    int is_the_socket_still_valid = 1;

    while (sent < size && is_the_socket_still_valid){
        s = send(this->_fd, &buffer[sent], size-sent, MSG_NOSIGNAL);
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

Socket* Socket::acceptPeer(){
    int peer_fd = accept(this->_fd, NULL, NULL);
    if (peer_fd == -1){
        return nullptr;
    }
    Socket* peer_socket = new Socket(peer_fd);
    return peer_socket;
}
