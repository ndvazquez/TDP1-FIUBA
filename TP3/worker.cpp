#include "worker.h"
#include "database_handler.h"
#include "socket.h"
#include "hash.h"
#include "encrypter.h"
#include "protocol.h"
#include "certificate_handler.h"
#include <iostream>

Worker::Worker(DatabaseHandler &db, Socket &socket, Key &key):
                _database(db),
                _peerSocket(std::move(socket)),
                _serverPrivateKey(key){}

void Worker::run(){
    try{
        Protocol protocol(_peerSocket);
        uint8_t action = protocol.receiveByte();
        if (action == 0){
            newCertificate(protocol);
        } else if (action == 1){
            revoke(protocol);
        }
        _isDead = true;
    } catch (...){
        _isDead = true;
        std::cout << "Something went wrong.\n";
    }    
}

void Worker::stop(){
    _peerSocket.shutdownAndClose();
}

void Worker::newCertificate(Protocol &protocol){
    //TODO: Codigo repetido con revoke, modularizar luego.
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
                                            _serverPrivateKey.getExponent(),
                                            _serverPrivateKey.getModulus());
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

void Worker::revoke(Protocol &protocol){
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
                                            _serverPrivateKey.getExponent(),
                                            _serverPrivateKey.getModulus());
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
