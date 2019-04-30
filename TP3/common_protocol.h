#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "common_socket.h"
#include "common_certificate_handler.h"
#include <memory>
#include <string>

#define NUMBER_SIZE 4
#define MODULUS_SIZE 2
#define EXPONENT_SIZE 1

class Protocol{
    protected:
    Socket &_socket;

    public:
    explicit Protocol(Socket &socket);
    Protocol& operator<<(const uint8_t &number);
    Protocol& operator<<(const uint16_t &number);
    Protocol& operator<<(const uint32_t &number);
    Protocol& operator<<(const std::string &value);
    Protocol& operator<<(const CertificateHandler &ch);

    Protocol& operator>>(uint8_t &number);
    Protocol& operator>>(uint16_t &number);
    Protocol& operator>>(uint32_t &number);
    Protocol& operator>>(std::string &value);
    Protocol& operator>>(CertificateHandler &ch);
};

#endif
