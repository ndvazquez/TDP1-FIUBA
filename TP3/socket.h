#ifndef SOCKET_H
#define SOCKET_H

#include <string>

class Socket{
    int _fd;
    int _wrapperGetAddrinfo(std::string host, std::string port, struct addrinfo **ptr,
            int passive);
    int _socketCreate(int ai_family, int ai_socktype, int ai_protocol);
    public:
    Socket();
    Socket(int fileDescriptor);
    ~Socket();
    //TODO: Aplicar manejo de excepciones a estas cosas.
    int bindAndListen(std::string host, std::string port);
    int connectToHost(std::string host, std::string port);
    Socket* acceptPeer();
    int receiveMessage(char *buffer, int size);
    int sendMessage(char *buffer, int size);
};

#endif
