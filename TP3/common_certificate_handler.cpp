#include "common_certificate_handler.h"
#include "common_key.h"
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

std::string CertificateHandler::_parseLine(std::string &line){
    std::string::iterator it;
    size_t tokenBegin = 0;
    size_t tokenLength = 0;
    for (it = line.begin(); it != line.end(); ++it){
        ++tokenBegin;
        if (*it == ':') break;
    }
    ++tokenBegin;
    for (it = line.begin() + tokenBegin; it != line.end() && *it != '('; ++it){
        ++tokenLength;
    }
    std::string token = line.substr(tokenBegin, tokenLength);
    return token;
}

CertificateHandler::CertificateHandler(){}

CertificateHandler::CertificateHandler(std::string &path){
    std::ifstream certFile;
    certFile.open(path);
    std::vector<std::string> lines;
    std::string line;
    while (getline(certFile, line, '\n')){
        lines.push_back(line);
    }
    _serial = std::stoi(_parseLine(lines[VECTOR_POS_SERIAL]));
    _subject = _parseLine(lines[VECTOR_POS_SUBJECT]);
    _issuer = _parseLine(lines[VECTOR_POS_ISSUER]);
    _s_date = _parseLine(lines[VECTOR_POS_SDATE]);
    _e_date = _parseLine(lines[VECTOR_POS_EDATE]);
    uint16_t modulus = std::stoi(_parseLine(lines[VECTOR_POS_MODULUS]));
    uint8_t exponent = std::stoi(_parseLine(lines[VECTOR_POS_EXP]));
    _key = Key(exponent, modulus);
}

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

uint32_t CertificateHandler::getSerial() const{
    return _serial;
}

uint16_t CertificateHandler::getKeyModulus() const{
    return _key.getModulus();
}

uint8_t CertificateHandler::getKeyExponent() const{
    return _key.getExponent();
}

std::string CertificateHandler::getSubject() const{
    return _subject;
}

std::string CertificateHandler::getIssuer() const{
    return _issuer;
}

std::string CertificateHandler::getStartingDate() const{
    return _s_date;
}

std::string CertificateHandler::getEndingDate() const{
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
