#include "protocol.h"
#include "socket.h"
#include "key.h"
#include "certificate_handler.h"
#include <arpa/inet.h>
#include <string>

#define NUMBER_SIZE 4
#define MODULUS_SIZE 2
#define EXPONENT_SIZE 1

Protocol::Protocol(Socket* socket) : _socket(socket){}
//TODO: Validar todos los send y que devuelva int esto.
void Protocol::sendCertificate(CertificateHandler &ch){
    uint32_t serial = htonl(ch.getSerial());
    _socket->sendMessage((char*) &serial, NUMBER_SIZE);
    
    std::string subject = ch.getSubject();
    uint32_t subject_size = htonl(subject.size());
    _socket->sendMessage((char*) &subject_size, NUMBER_SIZE);
    _socket->sendMessage(&subject[0], subject.size());

    std::string issuer = ch.getIssuer();
    uint32_t issuer_size = htonl(issuer.size());
    _socket->sendMessage((char*) &issuer_size, NUMBER_SIZE);
    _socket->sendMessage(&issuer[0], issuer.size());

    std::string s_date = ch.getStartingDate();
    uint32_t s_date_size = htonl(s_date.size());
    _socket->sendMessage((char*) &s_date_size, NUMBER_SIZE);
    _socket->sendMessage(&s_date[0], s_date.size());

    std::string e_date = ch.getEndingDate();
    uint32_t e_date_size = htonl(e_date.size());
    _socket->sendMessage((char*) &e_date_size, NUMBER_SIZE);
    _socket->sendMessage(&e_date[0], e_date.size());

    uint16_t modulus = htons(ch.getKeyModulus());
    _socket->sendMessage((char*) &modulus, MODULUS_SIZE);

    uint8_t exponent = ch.getKeyExponent();
    _socket->sendMessage((char*) &exponent, EXPONENT_SIZE);
}
//TODO: Validar todos los receives y que devuelva int esto.
void Protocol::receiveCertificate(CertificateHandler &ch){
    uint32_t serial;
    _socket->receiveMessage((char*) &serial, NUMBER_SIZE);
    serial = ntohl(serial);

    uint32_t subject_size;
    _socket->receiveMessage((char*) &subject_size, NUMBER_SIZE);
    subject_size = ntohl(subject_size);
    std::string subject(subject_size + 1, '\0');
    _socket->receiveMessage(&subject[0], subject_size);

    uint32_t issuer_size;
    _socket->receiveMessage((char*) &issuer_size, NUMBER_SIZE);
    issuer_size = ntohl(issuer_size);
    std::string issuer(issuer_size + 1, '\0');
    _socket->receiveMessage(&issuer[0], issuer_size);

    uint32_t s_date_size;
    _socket->receiveMessage((char*) &s_date_size, NUMBER_SIZE);
    s_date_size = ntohl(s_date_size);
    std::string s_date(s_date_size + 1, '\0');
    _socket->receiveMessage(&s_date[0], s_date_size);

    uint32_t e_date_size;
    _socket->receiveMessage((char*) &e_date_size, NUMBER_SIZE);
    e_date_size = ntohl(e_date_size);
    std::string e_date(e_date_size + 1, '\0');
    _socket->receiveMessage(&e_date[0], e_date_size);

    uint16_t modulus;
    _socket->receiveMessage((char*) &modulus, MODULUS_SIZE);
    modulus = ntohs(modulus);

    uint8_t exponent;
    _socket->receiveMessage((char*) &exponent, EXPONENT_SIZE);

    ch.setSerial(serial);
    ch.setSubject(subject);
    ch.setIssuer(issuer);
    ch.setStartingDate(s_date);
    ch.setEndingDate(e_date);
    ch.setKey(Key(exponent, modulus));
}
