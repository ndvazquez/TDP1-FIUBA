#include "server.h"
#include "key.h"
#include "database_handler.h"
#include "socket.h"
#include "protocol.h"
#include "hash.h"
#include "encrypter.h"
#include <iostream>

Server::Server(Key &privateKey, Key &publicKey,
                std::string &dbPath, Socket &acceptor){
    this->_privateKey = std::move(privateKey);
    this->_publicKey = std::move(publicKey);
    this->_database = DatabaseHandler(dbPath);
    this->_acceptor = std::move(acceptor);
}

void Server::run(){
    char c;
    bool flag = true;
    while (flag){
        Socket peerSocket = _acceptor.acceptPeer();
        if (!peerSocket.isValid()) continue;
        Protocol protocol(peerSocket);
        uint8_t action = protocol.receiveByte();
        if (action == 0){
            newCertificate(protocol);
        } else if (action == 1){
            revoke(protocol);
        }
        std::cin.get(c);
        flag = c != 'q';
    } 
}

void Server::newCertificate(Protocol &protocol){
    CertificateHandler ch;
    protocol.receiveNewCertificateData(ch);
    std::string subject = ch.getSubject();
    if (!_database.lookup(subject)){
        uint32_t serial = _database.getNextId();
        std::string issuer = "Taller de programacion 1";
        ch.setSerial(serial);
        ch.setIssuer(issuer);
        Key clientPubKey(ch.getKeyExponent(), ch.getKeyModulus());
        std::string cert = ch.createCertificate();
        Hash hasher;
        Encrypter encrypter;
        uint16_t hashed_cert = hasher(cert);
        
        uint32_t priv_step = encrypter.rsa(hashed_cert,
                                            _privateKey.getExponent(),
                                            _privateKey.getModulus());
        uint32_t pub_step = encrypter.rsa(priv_step,
                                            clientPubKey.getExponent(),
                                            clientPubKey.getModulus());
        _database.insert(subject, clientPubKey);
        protocol.sendByte(0);
        protocol.sendCertificate(ch);
        protocol.sendFingerPrint(pub_step);
        uint8_t response = protocol.receiveByte();
        if (response == 1) _database.remove(subject);
    } else{
        protocol.sendByte(1);
    }
}

void Server::revoke(Protocol &protocol){
    CertificateHandler ch;
    protocol.receiveCertificate(ch);
    std::string subject = ch.getSubject();
    if (_database.lookup(subject)){
        Key client_pub_key = _database.getPublicKey(subject);
        std::string cert = ch.createCertificate();
        Hash hasher;
        Encrypter encrypter;
        uint16_t hashed_cert = hasher(cert);
        uint32_t receivedFingerPrint = 0;
        protocol.receiveFingerPrint(&receivedFingerPrint);
        uint32_t priv_step = encrypter.rsa(receivedFingerPrint,
                                            _privateKey.getExponent(),
                                            _privateKey.getModulus());
        uint32_t pub_step = encrypter.rsa(priv_step,
                                            client_pub_key.getExponent(),
                                            client_pub_key.getModulus());
        if (hashed_cert == pub_step){
            _database.remove(subject);
            protocol.sendByte(0);
        } else {
            protocol.sendByte(2);
        }
    } else{
        protocol.sendByte(1);
    }
}



