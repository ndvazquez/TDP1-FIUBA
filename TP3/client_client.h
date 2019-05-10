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

class Client {
    Key _privateKey;
    Key _publicKey;
    Key _serverPublicKey;
    Socket _socket;
    std::string _filePath;
    std::string _parseLine(std::string &line);
    CertificateHandler _createCertificateFromFile();
    public:
    Client(Key &privateKey,
            Key &publicKey,
            Key &serverPublicKey,
            Socket &socket,
            std::string &filePath);
    void requestCertificateCreation();
    void requestCertificateRevocation();
};

#endif
