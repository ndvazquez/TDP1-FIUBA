#include "protocol.h"
#include "socket.h"
#include "key.h"
#include "certificate_handler.h"
#include <arpa/inet.h>
#include <string>
#include <memory>

Protocol::Protocol(Socket* socket) : _socket(socket){}
//TODO: Validar todos los send y que devuelva int esto.
void Protocol::sendCertificate(CertificateHandler &ch){
    uint32_t serial = htonl(ch.getSerial());
    _socket->sendMessage(&serial, NUMBER_SIZE);
    
    std::string subject = ch.getSubject();
    uint32_t subject_size = htonl(subject.size());
    _socket->sendMessage(&subject_size, NUMBER_SIZE);
    _socket->sendMessage(&subject[0], subject.size());

    std::string issuer = ch.getIssuer();
    uint32_t issuer_size = htonl(issuer.size());
    _socket->sendMessage(&issuer_size, NUMBER_SIZE);
    _socket->sendMessage(&issuer[0], issuer.size());

    std::string s_date = ch.getStartingDate();
    uint32_t s_date_size = htonl(s_date.size());
    _socket->sendMessage(&s_date_size, NUMBER_SIZE);
    _socket->sendMessage(&s_date[0], s_date.size());

    std::string e_date = ch.getEndingDate();
    uint32_t e_date_size = htonl(e_date.size());
    _socket->sendMessage(&e_date_size, NUMBER_SIZE);
    _socket->sendMessage(&e_date[0], e_date.size());

    uint16_t modulus = htons(ch.getKeyModulus());
    _socket->sendMessage(&modulus, MODULUS_SIZE);

    uint8_t exponent = ch.getKeyExponent();
    _socket->sendMessage(&exponent, EXPONENT_SIZE);
}
//TODO: Validar todos los receives y que devuelva int esto.
void Protocol::receiveCertificate(CertificateHandler &ch){
    uint32_t serial;
    _socket->receiveMessage(&serial, NUMBER_SIZE);
    serial = ntohl(serial);

    uint32_t subject_size;
    _socket->receiveMessage(&subject_size, NUMBER_SIZE);
    subject_size = ntohl(subject_size);
    std::string subject(subject_size + 1, '\0');
    _socket->receiveMessage(&subject[0], subject_size);

    uint32_t issuer_size;
    _socket->receiveMessage(&issuer_size, NUMBER_SIZE);
    issuer_size = ntohl(issuer_size);
    std::string issuer(issuer_size + 1, '\0');
    _socket->receiveMessage(&issuer[0], issuer_size);

    uint32_t s_date_size;
    _socket->receiveMessage(&s_date_size, NUMBER_SIZE);
    s_date_size = ntohl(s_date_size);
    std::string s_date(s_date_size + 1, '\0');
    _socket->receiveMessage(&s_date[0], s_date_size);

    uint32_t e_date_size;
    _socket->receiveMessage(&e_date_size, NUMBER_SIZE);
    e_date_size = ntohl(e_date_size);
    std::string e_date(e_date_size + 1, '\0');
    _socket->receiveMessage(&e_date[0], e_date_size);

    uint16_t modulus;
    _socket->receiveMessage(&modulus, MODULUS_SIZE);
    modulus = ntohs(modulus);

    uint8_t exponent;
    _socket->receiveMessage(&exponent, EXPONENT_SIZE);

    ch.setSerial(serial);
    ch.setSubject(subject);
    ch.setIssuer(issuer);
    ch.setStartingDate(s_date);
    ch.setEndingDate(e_date);
    ch.setKey(Key(exponent, modulus));
}

void Protocol::sendFingerPrint(uint32_t fingerPrint){
    uint32_t be_fp = htonl(fingerPrint);
    _socket->sendMessage(&be_fp, NUMBER_SIZE);
}

void Protocol::receiveFingerPrint(uint32_t *fingerPrint){
    uint32_t be_fp;
    _socket->receiveMessage(&be_fp, NUMBER_SIZE);
    *fingerPrint = ntohl(be_fp);
}
