#ifndef CERTIFICATE_HANDLER_H
#define CERTIFICATE_HANDLER_H
#include <string>
#include "common_key.h"
#define VECTOR_POS_SERIAL 1
#define VECTOR_POS_SUBJECT 2
#define VECTOR_POS_ISSUER 3
#define VECTOR_POS_SDATE 5
#define VECTOR_POS_EDATE 6
#define VECTOR_POS_MODULUS 8
#define VECTOR_POS_EXP 9

class CertificateHandler{
    uint32_t _serial;
    std::string _subject;
    std::string _issuer;
    std::string _s_date;
    std::string _e_date;
    Key _key;
    std::string _to_hexadecimal(int number, int width);
    std::string _parseLine(std::string &line);

    public:
    CertificateHandler();
    explicit CertificateHandler(std::string &path);
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
