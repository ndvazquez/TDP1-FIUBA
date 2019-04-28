#ifndef SOCKET_H
#define SOCKET_H

#include <string>

class Socket{
    int _fd;
    explicit Socket(int fileDescriptor);
    int _wrapperGetAddrinfo(std::string host, std::string port, 
            struct addrinfo **ptr, int passive);
    int _socketCreate(int ai_family, int ai_socktype, int ai_protocol);
    public:
    Socket();
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);
    Socket(const Socket&) =delete;
    Socket& operator=(const Socket&) =delete;
    ~Socket();
    bool isValid();
    
    //TODO: Aplicar manejo de excepciones a estas cosas.
    int bindAndListen(std::string host, std::string port);
    int connectToHost(std::string host, std::string port);
    Socket acceptPeer();
    int receiveMessage(void *buffer, int size);
    int sendMessage(void *buffer, int size);
    void shutdownAndClose();
};

#endif
