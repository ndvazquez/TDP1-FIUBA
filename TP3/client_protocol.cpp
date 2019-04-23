#include <string>
#include "client_protocol.h"
#include <arpa/inet.h>

void ClientProtocol::requestNewCertificate(){
    uint8_t command_new = 0;
    _socket->sendMessage(&command_new, EXPONENT_SIZE);
}

void ClientProtocol::requestRevokeCertificate(){
    uint8_t command_revoke = 1;
    _socket->sendMessage(&command_revoke, EXPONENT_SIZE);
}

void ClientProtocol::sendNewCertificateData(std::string subject,
                                std::string s_date,
                                std::string e_date,
                                Key publicKey){
    uint32_t subject_size = htonl(subject.size());
    _socket->sendMessage(&subject_size, NUMBER_SIZE);
    _socket->sendMessage(&subject[0], subject.size());
    
    uint16_t modulus = htonl(publicKey.getModulus());
    _socket->sendMessage(&modulus, MODULUS_SIZE);

    uint8_t exponent = publicKey.getExponent();
    _socket->sendMessage(&exponent, EXPONENT_SIZE);

    uint32_t s_date_size = htonl(s_date.size());
    _socket->sendMessage(&s_date_size, NUMBER_SIZE);
    _socket->sendMessage(&s_date[0], s_date.size());

    uint32_t e_date_size = htonl(e_date.size());
    _socket->sendMessage(&e_date_size, NUMBER_SIZE);
    _socket->sendMessage(&e_date[0], e_date.size());
}