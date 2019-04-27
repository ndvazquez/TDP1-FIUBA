#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "server_thread.h"
#include "common_socket.h"
#include "server_database_handler.h"
#include "common_key.h"
#include <vector>

class Acceptor : public Thread{
    DatabaseHandler &_database;
    Socket &_socket;
    Key &_serverPrivateKey;
    std::vector<Thread*> _workers;
    bool _keepRunning;
    public:
    Acceptor(DatabaseHandler &db, Socket &socket, Key &key);
    void run();
    void stop();
};

#endif
