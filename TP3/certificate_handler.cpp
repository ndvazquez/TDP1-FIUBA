#include "certificate_handler.h"
#include "key.h"
#include <string>
#include <iomanip>

CertificateHandler::CertificateHandler(){}

CertificateHandler::CertificateHandler(unsigned int serial, 
                            std::string subject,
                            std::string issuer,
                            std::string s_date,
                            std::string e_date,
                            Key key) :
                            _serial(serial),
                            _subject(subject),
                            _issuer(issuer),
                            _s_date(s_date),
                            _e_date(e_date),
                            _key(key){}

std::string CertificateHandler::_to_hexadecimal(int number, int width){
    std::stringstream result;
    result << "0x" << std::hex << std::setw(width) << std::setfill('0');
    result << number;
    return result.str();
}

std::string CertificateHandler::createCertificate(){
    std::string serial_hex = _to_hexadecimal(_serial, 8);
    std::string mod_hex = _to_hexadecimal(_key.getModulus(), 4);
    std::string exp_hex = _to_hexadecimal(_key.getExponent(), 2);

    std::stringstream certificate;
    certificate << "certificate:\n\tserial number: " << _serial << " ";
    certificate << serial_hex << "\n\tsubject: " << _subject; 
    certificate << "\n\tissuer: " << _issuer << "\n\tvalidity:\n\t\t";
    certificate << "not before: " << _s_date << "\n\t\t" << "not after: ";
    certificate << _e_date << "\n\t" << "subject public key info:\n\t\t";
    certificate << "modulus: " << _key.getModulus() << " " ;
    certificate << mod_hex << "\n\t\texponent: ";
    certificate << (int) _key.getExponent() << " " << exp_hex;

    return certificate.str();
}
