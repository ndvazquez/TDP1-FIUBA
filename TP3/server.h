#ifndef SERVER_H
#define SERVER_h
#include "key.h"
#include "database_handler.h"
#include "socket.h"
#include "protocol.h"
#include <string>

class Server{
    Key _privateKey;
    Key _publicKey;
    DatabaseHandler _database;
    Socket _acceptor;
    public:
    Server(Key &privateKey,
            Key &publicKey,
            std::string &dbPath,
            Socket &acceptor);
    void run();
    void revoke(Protocol &protocol);
    void newCertificate(Protocol &protocol);
};

#endif