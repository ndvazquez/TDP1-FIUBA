#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "socket.h"
#include "certificate_handler.h"
#include <memory>

#define NUMBER_SIZE 4
#define MODULUS_SIZE 2
#define EXPONENT_SIZE 1

class Protocol{
    void _sendUnsignedInteger(uint32_t number);
    void _sendString(std::string stringToSend);
    void _sendUnsignedShort(uint16_t number);
    uint32_t _receiveUnsignedInteger();
    std::string _receiveString();
    uint16_t _receiveUnsignedShort();
    
    protected:
    std::unique_ptr<Socket> _socket;

    public:
    Protocol(Socket* socket);
    void sendFingerPrint(uint32_t fingerPrint);
    void receiveFingerPrint(uint32_t *fingerPrint);
    void receiveCertificate(CertificateHandler &ch);
    void sendCertificate(CertificateHandler &ch);
    void sendNewCertificateData(std::string subject,
                                std::string s_date,
                                std::string e_date,
                                Key publicKey);
    void receiveNewCertificateData(CertificateHandler &ch);
    void sendByte(uint8_t byte);
    uint8_t receiveByte();
};

#endif