#ifndef SERVER_H
#define SERVER_h
#include "common_key.h"
#include "server_database_handler.h"
#include "common_socket.h"
#include <string>

class Server{
    Key _privateKey;
    Key _publicKey;
    DatabaseHandler _database;
    public:
    Server(Key &privateKey,
            Key &publicKey,
            std::string &dbPath);
    void run(std::string &host, std::string &service);
};

#endif
