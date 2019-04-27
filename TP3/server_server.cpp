#include "server_server.h"
#include "common_key.h"
#include "server_database_handler.h"
#include "common_socket.h"
#include "server_worker.h"
#include "server_acceptor.h"
#include <iostream>

Server::Server(Key &privateKey, Key &publicKey,
                std::string &dbPath, Socket &acceptor){
    this->_privateKey = std::move(privateKey);
    this->_publicKey = std::move(publicKey);
    this->_database.initializeData(dbPath);
    this->_acceptor = std::move(acceptor);
}

void Server::run(){
    char c = '*';
    Acceptor acceptorThread(_database, _acceptor, _privateKey);
    acceptorThread.start();
    while (c != 'q'){
        std::cin.get(c);
    }
    acceptorThread.stop();
    acceptorThread.join(); 
}
