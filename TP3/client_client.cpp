#include "client_client.h"
#include "common_protocol.h"
#include "common_certificate_handler.h"
#include "common_key.h"
#include "common_socket.h"
#include "common_hash.h"
#include "common_encrypter.h"
#include "client_date_handler.h"
#include <vector>
#include <string>
#include <fstream>
#define MAX_REQUEST_INFO_LINES 3

Client::Client(Key &privateKey,
            Key &publicKey,
            Key &serverPublicKey,
            Socket &socket,
            std::string &filePath) : 
            _privateKey(std::move(privateKey)),
            _publicKey(std::move(publicKey)),
            _serverPublicKey(std::move(serverPublicKey)),
            _socket(std::move(socket)),
            _filePath(filePath){}

std::string Client::_parseLine(std::string &line){
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

CertificateHandler Client::_createCertificateFromFile(){
    std::ifstream certificateFile;
    //TODO: Tirar exception si falla.
    certificateFile.open(_filePath);
    std::string line;
    uint32_t serial = 0;
    std::string subject;
    std::string issuer;
    std::string s_date;
    std::string e_date;
    uint16_t modulus = 0;
    uint8_t exponent = 0;
    int counter = 0;
    while (getline(certificateFile, line, '\n')){
        if (counter == 1) serial = std::stoi(_parseLine(line));
        if (counter == 2) subject = _parseLine(line);
        if (counter == 3) issuer = _parseLine(line);
        if (counter == 5) s_date  = _parseLine(line);
        if (counter == 6) e_date = _parseLine(line);
        if (counter == 8) modulus = std::stoi(_parseLine(line));
        if (counter == 9) exponent = std::stoi(_parseLine(line));
        ++counter;
    }
    Key key(exponent, modulus);
    CertificateHandler ch(serial, subject, issuer, s_date, e_date, key);
    return ch;
}

void Client::requestCertificateCreation(){
    std::ifstream certificateInfo;
    // Tirar excepción si no se abre bien.
    certificateInfo.open(_filePath);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(certificateInfo, line, '\n')){
        lines.push_back(std::move(line));
    }
    std::string subject = std::move(lines[0]);
    std::string s_date;
    std::string e_date;
    if (lines.size() == MAX_REQUEST_INFO_LINES){
        s_date = std::move(lines[1]);
        e_date = std::move(lines[2]);
    } else {
        DateHandler dh;
        dh.setStartAndEndDate(s_date, e_date);
    }

    Protocol protocol(_socket);
    protocol.sendByte(0);
    protocol.sendNewCertificateData(subject, s_date, e_date, _publicKey);
    int new_status = protocol.receiveByte();
    if (new_status != 0) {
        std::cout << "Error: ya existe un certificado.\n";
        return;
    }
    CertificateHandler certificateHandler;
    protocol.receiveCertificate(certificateHandler);
    uint32_t receivedFingerprint;
    protocol.receiveFingerPrint(&receivedFingerprint);
    std::string cert = certificateHandler.createCertificate();
    Hash hasher;
    Encrypter encrypter;
    uint16_t hashed_cert = hasher(cert);
    uint32_t privateStep = encrypter.rsa(receivedFingerprint,
                                        _privateKey.getExponent(),
                                        _privateKey.getModulus());
    uint32_t publicStep = encrypter.rsa(privateStep,
                                        _serverPublicKey.getExponent(),
                                        _serverPublicKey.getModulus());
    std::cout << "Huella del servidor: " << receivedFingerprint << std::endl;
    std::cout << "Hash del servidor: " << publicStep << std::endl;
    std::cout << "Hash calculado: " << hashed_cert << std::endl;
    
    uint8_t response = publicStep == hashed_cert ? 0 : 1;
    if (response == 1){
        std::cout << "Error: los hashes no coinciden.\n";
    }
    if (response == 0){
        std::ofstream outputFile;
        std::string certFileName = subject + ".cert";
        outputFile.open(certFileName);
        outputFile << cert;
    }
    protocol.sendByte(response);
}

void Client::requestCertificateRevocation(){
    Protocol protocol(_socket);
    CertificateHandler ch = _createCertificateFromFile();
    std::string cert = ch.createCertificate();
    Hash hasher;
    Encrypter encrypter;
    uint16_t hashed_cert = hasher(cert);
    uint32_t privateStep = encrypter.rsa(hashed_cert,
                                        _privateKey.getExponent(),
                                        _privateKey.getModulus());
    uint32_t publicStep = encrypter.rsa(privateStep,
                                        _serverPublicKey.getExponent(),
                                        _serverPublicKey.getModulus());
    
    protocol.sendByte(1);
    protocol.sendCertificate(ch);
    protocol.sendFingerPrint(publicStep);
    std::cout << "Hash calculado: " << hashed_cert << std::endl;
    std::cout << "Hash encriptado con la clave privada: ";
    std::cout << privateStep << std::endl;
    std::cout << "Huella enviada: " << publicStep << std::endl;
    uint8_t serverResponse = protocol.receiveByte();
    if (serverResponse == 1) std::cout << "Error: usuario no registrado.\n";
    if (serverResponse == 2) std::cout << "Error: los hashes no coinciden.\n";
}
