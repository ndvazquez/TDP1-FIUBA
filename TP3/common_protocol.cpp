#include "common_protocol.h"
#include "common_socket.h"
#include "common_key.h"
#include "common_certificate_handler.h"
#include <arpa/inet.h>
#include <string>
#include <memory>

Protocol::Protocol(Socket &socket) : _socket(socket){}

Protocol& Protocol::operator<<(const uint8_t &number){
    _socket.sendMessage(&number, EXPONENT_SIZE);
    return *this;
}

Protocol& Protocol::operator<<(const uint16_t &number){
    uint16_t n_number = htons(number);
    _socket.sendMessage(&n_number, MODULUS_SIZE);
    return *this;
}

Protocol& Protocol::operator<<(const uint32_t &number){
    uint32_t n_number = htonl(number);
    _socket.sendMessage(&n_number, NUMBER_SIZE);
    return *this;
}

Protocol& Protocol::operator<<(const std::string &value){
    uint32_t string_size = htonl(value.size());
    _socket.sendMessage(&string_size, NUMBER_SIZE);
    _socket.sendMessage(&value[0], value.size());
    return *this;
}

Protocol& Protocol::operator<<(const CertificateHandler &ch){
    *this << ch.getSerial();
    *this << ch.getSubject();
    *this << ch.getIssuer();
    *this << ch.getStartingDate();
    *this << ch.getEndingDate();
    *this << ch.getKeyModulus();
    *this << ch.getKeyExponent();
    return *this;
}

Protocol& Protocol::operator>>(uint8_t &number){
    _socket.receiveMessage(&number, EXPONENT_SIZE);
    return *this;
}

Protocol& Protocol::operator>>(uint16_t &number){
    _socket.receiveMessage(&number, MODULUS_SIZE);
    number = ntohs(number);
    return *this;
}
Protocol& Protocol::operator>>(uint32_t &number){
    _socket.receiveMessage(&number, NUMBER_SIZE);
    number = ntohl(number);
    return *this;
}

Protocol& Protocol::operator>>(std::string &value){
    uint32_t string_size;
    _socket.receiveMessage(&string_size, NUMBER_SIZE);
    string_size = ntohl(string_size);
    std::string receivedString(string_size, '\0');
    _socket.receiveMessage(&receivedString[0], string_size);
    value = std::move(receivedString);
    return *this;
}

Protocol& Protocol::operator>>(CertificateHandler &ch){
    uint32_t serial;
    *this >> serial;
    std::string subject;
    *this >> subject;
    std::string issuer;
    *this >> issuer;
    std::string s_date;
    *this >> s_date;
    std::string e_date;
    *this >> e_date;
    uint16_t modulus;
    *this >> modulus;
    uint8_t exponent;
    *this >> exponent;
    ch.setSerial(serial);
    ch.setSubject(subject);
    ch.setIssuer(issuer);
    ch.setStartingDate(s_date);
    ch.setEndingDate(e_date);
    ch.setKey(Key(exponent, modulus));
    return *this;
}
