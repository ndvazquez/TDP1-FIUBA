#ifndef CERTIFICATE_ASSEMBLER_H
#define CERTIFICATE_ASSEMBLER_H
#include <string>

class CertificateAssembler{
    std::string to_hexadecimal(int number, int width);

    public:
    CertificateAssembler();
    std::string operator()(unsigned int serial, 
                            std::string subject,
                            std::string issuer,
                            std::string s_date,
                            std::string e_date,
                            unsigned int mod,
                            unsigned int client_pub_exp);
};

#endif
