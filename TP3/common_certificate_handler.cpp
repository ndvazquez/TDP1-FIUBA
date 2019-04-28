#include "common_certificate_handler.h"
#include "common_key.h"
#include <string>
#include <iomanip>
#include <sstream>

CertificateHandler::CertificateHandler(){}

CertificateHandler::CertificateHandler(uint32_t serial, 
                            std::string subject,
                            std::string issuer,
                            std::string s_date,
                            std::string e_date,
                            Key &key) :
                            _serial(serial),
                            _subject(subject),
                            _issuer(issuer),
                            _s_date(s_date),
                            _e_date(e_date),
                            _key(std::move(key)){}

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
    certificate << "certificate:\n\tserial number: " << _serial << " (";
    certificate << serial_hex << ")\n\tsubject: " << _subject; 
    certificate << "\n\tissuer: " << _issuer << "\n\tvalidity:\n\t\t";
    certificate << "not before: " << _s_date << "\n\t\t" << "not after: ";
    certificate << _e_date << "\n\t" << "subject public key info:\n\t\t";
    certificate << "modulus: " << _key.getModulus() << " (";
    certificate << mod_hex << ")\n\t\texponent: ";
    certificate << (int) _key.getExponent() << " (" << exp_hex << ")";

    return certificate.str();
}

uint32_t CertificateHandler::getSerial(){
    return _serial;
}

uint16_t CertificateHandler::getKeyModulus(){
    return _key.getModulus();
}

uint8_t CertificateHandler::getKeyExponent(){
    return _key.getExponent();
}

std::string CertificateHandler::getSubject(){
    return _subject;
}

std::string CertificateHandler::getIssuer(){
    return _issuer;
}

std::string CertificateHandler::getStartingDate(){
    return _s_date;
}

std::string CertificateHandler::getEndingDate(){
    return _e_date;
}

void CertificateHandler::setSerial(uint32_t &serial){
    this->_serial = serial;
}

void CertificateHandler::setSubject(std::string &subject){
    this->_subject = subject;
}
void CertificateHandler::setIssuer(std::string &issuer){
    this->_issuer = issuer;
}
void CertificateHandler::setStartingDate(std::string &s_date){
    this->_s_date = s_date;
}
void CertificateHandler::setEndingDate(std::string &e_date){
    this->_e_date = e_date;
}
void CertificateHandler::setKey(Key key){
    this->_key = std::move(key);
}
