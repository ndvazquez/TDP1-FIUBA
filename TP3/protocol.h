#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "socket.h"
#include "certificate_handler.h"
#include <memory>

#define NUMBER_SIZE 4
#define MODULUS_SIZE 2
#define EXPONENT_SIZE 1

class Protocol{
    protected:
    std::unique_ptr<Socket> _socket;
    public:
    Protocol(Socket* socket);
    void sendFingerPrint(uint32_t fingerPrint);
    void receiveFingerPrint(uint32_t *fingerPrint);
    void receiveCertificate(CertificateHandler &ch);
    void sendCertificate(CertificateHandler &ch); 
};

#endif