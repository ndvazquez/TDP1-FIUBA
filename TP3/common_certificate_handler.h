#ifndef CERTIFICATE_HANDLER_H
#define CERTIFICATE_HANDLER_H
#include <string>
#include "common_key.h"

class CertificateHandler{
    uint32_t _serial;
    std::string _subject;
    std::string _issuer;
    std::string _s_date;
    std::string _e_date;
    Key _key;

    std::string _to_hexadecimal(int number, int width);

    public:
    CertificateHandler();
    CertificateHandler(uint32_t serial, 
                            std::string subject,
                            std::string issuer,
                            std::string s_date,
                            std::string e_date,
                            Key &key);
    //TODO: Implementar constructor por movimiento.
    std::string createCertificate();
    uint32_t getSerial() const;
    std::string getSubject() const;
    std::string getIssuer() const;
    std::string getStartingDate() const;
    std::string getEndingDate() const;
    uint8_t getKeyExponent() const;
    uint16_t getKeyModulus() const;
    void setSerial(uint32_t &serial);
    void setSubject(std::string &subject);
    void setIssuer(std::string &issuer);
    void setStartingDate(std::string &s_date);
    void setEndingDate(std::string &e_date);
    void setKey(Key key);
};

#endif
