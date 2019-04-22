#ifndef CERTIFICATE_HANDLER_H
#define CERTIFICATE_HANDLER_H
#include <string>
#include "key.h"

class CertificateHandler{
    unsigned int _serial;
    std::string _subject;
    std::string _issuer;
    std::string _s_date;
    std::string _e_date;
    Key _key;

    std::string _to_hexadecimal(int number, int width);

    public:
    CertificateHandler();
    CertificateHandler(unsigned int serial, 
                            std::string subject,
                            std::string issuer,
                            std::string s_date,
                            std::string e_date,
                            Key key);
    //TODO: Implementar constructor por movimiento.
    std::string createCertificate();
};

#endif
