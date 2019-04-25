#include "protocol.h"
#include "socket.h"
#include "key.h"
#include "certificate_handler.h"
#include <arpa/inet.h>
#include <string>
#include <memory>

Protocol::Protocol(Socket &socket){
    this->_socket = std::move(socket);
}

void Protocol::_sendUnsignedInteger(uint32_t number){
    uint32_t n_number = htonl(number);
    _socket.sendMessage(&n_number, NUMBER_SIZE);
}

void Protocol::_sendString(std::string stringToSend){
    uint32_t string_size = htonl(stringToSend.size());
    _socket.sendMessage(&string_size, NUMBER_SIZE);
    _socket.sendMessage(&stringToSend[0], stringToSend.size());
}

void Protocol::_sendUnsignedShort(uint16_t number){
    uint16_t n_number = htons(number);
    _socket.sendMessage(&n_number, MODULUS_SIZE);
}

void Protocol::sendByte(uint8_t byte){
    _socket.sendMessage(&byte, EXPONENT_SIZE);
}

std::string Protocol::_receiveString(){
    uint32_t string_size;
    _socket.receiveMessage(&string_size, NUMBER_SIZE);
    string_size = ntohl(string_size);
    std::string receivedString(string_size, '\0');
    _socket.receiveMessage(&receivedString[0], string_size);
    return receivedString;
}

uint32_t Protocol::_receiveUnsignedInteger(){
    uint32_t number;
    _socket.receiveMessage(&number, NUMBER_SIZE);
    number = ntohl(number);
    return number;
}

uint16_t Protocol::_receiveUnsignedShort(){
    uint16_t number;
    _socket.receiveMessage(&number, MODULUS_SIZE);
    number = ntohs(number);
    return number;
}

uint8_t Protocol::receiveByte(){
    uint8_t number;
    _socket.receiveMessage(&number, EXPONENT_SIZE);
    return number;
}

void Protocol::sendCertificate(CertificateHandler &ch){
    _sendUnsignedInteger(ch.getSerial());
    _sendString(ch.getSubject());
    _sendString(ch.getIssuer());
    _sendString(ch.getStartingDate());
    _sendString(ch.getEndingDate());
    _sendUnsignedShort(ch.getKeyModulus());
    sendByte(ch.getKeyExponent());
}

void Protocol::receiveCertificate(CertificateHandler &ch){
    uint32_t serial = _receiveUnsignedInteger();
    std::string subject = _receiveString();
    std::string issuer = _receiveString();
    std::string s_date = _receiveString();
    std::string e_date = _receiveString();
    uint16_t modulus = _receiveUnsignedShort();
    uint8_t exponent = receiveByte();

    ch.setSerial(serial);
    ch.setSubject(subject);
    ch.setIssuer(issuer);
    ch.setStartingDate(s_date);
    ch.setEndingDate(e_date);
    ch.setKey(Key(exponent, modulus));
}

void Protocol::sendFingerPrint(uint32_t fingerPrint){
    _sendUnsignedInteger(fingerPrint);
}

void Protocol::receiveFingerPrint(uint32_t *fingerPrint){
    uint32_t be_fp = _receiveUnsignedInteger();
    *fingerPrint = be_fp;
}

void Protocol::sendNewCertificateData(std::string subject,
                                std::string s_date,
                                std::string e_date,
                                Key publicKey){
    _sendString(subject);
    _sendUnsignedShort(publicKey.getModulus());
    sendByte(publicKey.getExponent());
    _sendString(s_date);
    _sendString(e_date);
}

void Protocol::receiveNewCertificateData(CertificateHandler &ch){
    std::string subject = _receiveString();
    uint16_t modulus = _receiveUnsignedShort();
    uint8_t exponent = receiveByte();
    std::string s_date = _receiveString();
    std::string e_date = _receiveString();

    ch.setSubject(subject);
    ch.setStartingDate(s_date);
    ch.setEndingDate(e_date);
    ch.setKey(Key(exponent, modulus));
}
