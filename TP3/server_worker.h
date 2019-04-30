#ifndef WORKER_H
#define WORKER_H

#include "server_database_handler.h"
#include "common_socket.h"
#include "server_thread.h"
#include "common_protocol.h"
#include "common_key.h"

class Worker : public Thread{
    DatabaseHandler &_database;
    Socket _peerSocket;
    Key &_serverPrivateKey;
    void newCertificate(Protocol &protocol);
    void revoke(Protocol &protocol);
    uint32_t _handleEncryption(uint32_t hashed_value,
                            uint8_t exponent,
                            uint16_t modulus);
    public:
    Worker(DatabaseHandler &db, Socket &socket, Key &key);
    void run();
    void stop();
};

#endif
