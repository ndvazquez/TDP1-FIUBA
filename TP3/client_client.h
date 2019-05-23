#ifndef CLIENT_H
#define CLIENT_H
#include "common_key.h"
#include "common_socket.h"
#include "common_certificate_handler.h"
#include <string>
#define MAX_REQUEST_INFO_LINES 3
#define PARAMS_SIZE 7
#define MODE_NEW "new"
#define MODE_REVOKE "revoke"
#define HASH_ERROR 1
#define HASH_OK 0
#define CERTIFICATE_CREATED 0
#define NEW_CERTIFICATE 0
#define REVOKE_CERTIFICATE 1
#define SERVER_HASH_ERROR 2
#define SERVER_UNREGISTERED_USER 1
#define CERT_INFO_POS_SUBJECT 0
#define CERT_INFO_POS_STARTING_DATE 1
#define CERT_INFO_POS_ENDING_DATE 2

class Client {
    Key _privateKey;
    Key _publicKey;
    Key _serverPublicKey;
    std::string _filePath;
    Socket _socket;
    public:
    Client(Key &privateKey,
            Key &publicKey,
            Key &serverPublicKey,
            std::string &filePath,
            std::string &host,
            std::string &port);
    void requestCertificateCreation();
    void requestCertificateRevocation();
};

#endif
