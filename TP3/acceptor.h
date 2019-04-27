#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "thread.h"
#include "socket.h"
#include "database_handler.h"
#include "key.h"
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
