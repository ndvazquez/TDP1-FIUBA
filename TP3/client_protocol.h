#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H
#include "protocol.h"
#include <string>
#include "key.h"

class ClientProtocol : public Protocol{
    public:
    void requestNewCertificate();
    void requestRevokeCertificate();
    void sendNewCertificateData(std::string subject, std::string s_date,
                                std::string e_date, Key publicKey);
};

#endif