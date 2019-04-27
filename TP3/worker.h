#ifndef WORKER_H
#define WORKER_H

#include "database_handler.h"
#include "socket.h"
#include "thread.h"
#include "protocol.h"
#include "key.h"

class Worker : public Thread{
    DatabaseHandler &_database;
    Socket _peerSocket;
    Key &_serverPrivateKey;
    void newCertificate(Protocol &protocol);
    void revoke(Protocol &protocol);
    uint32_t _handleEncryption(uint32_t hashed_value, Key &clientPubKey);
    public:
    Worker(DatabaseHandler &db, Socket &socket, Key &key);
    void run();
    void stop();
};

#endif
