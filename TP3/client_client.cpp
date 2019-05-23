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
#include <exception>

Client::Client(Key &privateKey,
            Key &publicKey,
            Key &serverPublicKey,
            std::string &filePath,
            std::string &host,
            std::string &port) : 
            _privateKey(std::move(privateKey)),
            _publicKey(std::move(publicKey)),
            _serverPublicKey(std::move(serverPublicKey)),
            _filePath(filePath){
    int status = _socket.connectToHost(host, port);
    if (status == -1) throw std::runtime_error("Couldn't connect to host.\n");
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
    std::string subject = std::move(lines[CERT_INFO_POS_SUBJECT]);
    std::string s_date;
    std::string e_date;
    if (lines.size() == MAX_REQUEST_INFO_LINES){
        s_date = std::move(lines[CERT_INFO_POS_STARTING_DATE]);
        e_date = std::move(lines[CERT_INFO_POS_ENDING_DATE]);
    } else {
        DateHandler dh;
        dh.setStartAndEndDate(s_date, e_date);
    }

    Protocol protocol(_socket);
    // We send all the required data to create a new cert.
    uint8_t newCertificate = NEW_CERTIFICATE;
    protocol << newCertificate;
    protocol << subject;
    protocol << _publicKey.getModulus();
    protocol << _publicKey.getExponent();
    protocol << s_date;
    protocol << e_date;
    
    uint8_t new_status;
    protocol >> new_status;
    if (new_status != CERTIFICATE_CREATED) {
        std::cout << "Error: ya existe un certificado.\n";
        return;
    }
    CertificateHandler certificateHandler;
    protocol >> certificateHandler;
    uint32_t receivedFingerprint;
    protocol >> receivedFingerprint;
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

    uint8_t response = publicStep == hashed_cert ? HASH_OK : HASH_ERROR;
    if (response == HASH_ERROR){
        std::cout << "Error: los hashes no coinciden.\n";
    }
    if (response == HASH_OK){
        std::ofstream outputFile;
        std::string certFileName = subject + ".cert";
        outputFile.open(certFileName);
        outputFile << cert;
    }
    protocol << response;
}

void Client::requestCertificateRevocation(){
    Protocol protocol(_socket);
    CertificateHandler ch(_filePath);
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
    uint8_t revokeCertificate = REVOKE_CERTIFICATE;
    protocol << revokeCertificate;
    protocol << ch;
    protocol << publicStep;
    std::cout << "Hash calculado: " << hashed_cert << std::endl;
    std::cout << "Hash encriptado con la clave privada: ";
    std::cout << privateStep << std::endl;
    std::cout << "Huella enviada: " << publicStep << std::endl;
    uint8_t serverResponse;
    protocol >> serverResponse;
    if (serverResponse == SERVER_UNREGISTERED_USER) {
        std::cout << "Error: usuario no registrado.\n";
    }
    if (serverResponse == SERVER_HASH_ERROR) {
        std::cout << "Error: los hashes no coinciden.\n";
    }
}
