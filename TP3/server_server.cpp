#include "server_server.h"
#include "common_key.h"
#include "server_database_handler.h"
#include "common_socket.h"
#include "server_worker.h"
#include "server_acceptor.h"
#include <string>
#include <iostream>
#include <exception>

Server::Server(Key &privateKey, Key &publicKey,
                std::string &dbPath){
    this->_privateKey = std::move(privateKey);
    this->_publicKey = std::move(publicKey);
    this->_database.initializeData(dbPath);
}

void Server::run(std::string &host, std::string &service){
    char c = '*';
    Socket acceptor;
    int status = acceptor.bindAndListen(host, service);
    if (status == SOCKET_BIND_ERROR){
        throw std::runtime_error("Couldn't bind to service.\n");
    }
    Acceptor acceptorThread(_database, acceptor, _privateKey);
    acceptorThread.start();
    while (c != 'q'){
        std::cin.get(c);
    }
    acceptorThread.stop();
    acceptorThread.join(); 
}
