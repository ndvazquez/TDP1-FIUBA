#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "socket.h"
#include "certificate_handler.h"

class Protocol{
    Socket* _socket;

    public:
    Protocol(Socket* socket);
    void receiveCertificate(CertificateHandler &ch);
    void sendCertificate(CertificateHandler &ch);  
};

#endif