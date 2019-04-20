#include "certificate_assembler.h"
#include <string>
#include <iomanip>

CertificateAssembler::CertificateAssembler(){}

std::string CertificateAssembler::to_hexadecimal(int number, int width){
    std::stringstream result;
    result << "0x" << std::hex << std::setw(width) << std::setfill('0');
    result << number;
    return result.str();
}

std::string CertificateAssembler::operator()(unsigned int serial, 
                            std::string subject,
                            std::string issuer,
                            std::string s_date,
                            std::string e_date,
                            unsigned int mod,
                            unsigned int client_pub_exp){
    std::string serial_hex = to_hexadecimal(serial, 8);
    std::string mod_hex = to_hexadecimal(mod, 4);
    std::string exp_hex = to_hexadecimal(client_pub_exp, 2);

    std::stringstream certificate;
    certificate << "certificate:\n\tserial number: " << serial << " ";
    certificate << serial_hex << "\n\tsubject: " << subject; 
    certificate << "\n\tissuer: " << issuer << "\n\tvalidity:\n\t\t";
    certificate << "not before: " << s_date << "\n\t\t" << "not after: ";
    certificate << e_date << "\n\t" << "subject public key info:\n\t\t";
    certificate << "modulus: " << mod << " " << mod_hex << "\n\t\texponent: ";
    certificate << client_pub_exp << " " << exp_hex;

    return certificate.str();
}
