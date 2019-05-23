#ifndef WORKER_H
#define WORKER_H

#include "server_database_handler.h"
#include "common_socket.h"
#include "server_thread.h"
#include "common_protocol.h"
#include "common_key.h"

#define REQUEST_NEW_CERT 0
#define REQUEST_REVOKE_CERT 1
#define CERT_CREATION_SUCCESS 0
#define SUBJECT_ALREADY_EXISTS 1
#define CLIENT_INVALID_HASH 1
#define CERT_REVOCATION_SUCCESS 0
#define INVALID_HASH 2
#define SUBJECT_DOESNT_EXISTS 1
#define DEFAULT_CERT_ISSUER "Taller de programacion 1"

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
