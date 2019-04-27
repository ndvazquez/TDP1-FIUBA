#include "worker.h"
#include "database_handler.h"
#include "socket.h"
#include "hash.h"
#include "encrypter.h"
#include "protocol.h"
#include "certificate_handler.h"
#include <iostream>
#include <stdexcept>

Worker::Worker(DatabaseHandler &db, Socket &socket, Key &key):
                _database(db),
                _peerSocket(std::move(socket)),
                _serverPrivateKey(key){}

uint32_t Worker::_handleEncryption(uint32_t hashed_value, Key &clientPubKey){
    Encrypter encrypter;
    uint32_t priv_step = encrypter.rsa(hashed_value,
                                            _serverPrivateKey.getExponent(),
                                            _serverPrivateKey.getModulus());
    uint32_t pub_step = encrypter.rsa(priv_step,
                                        clientPubKey.getExponent(),
                                        clientPubKey.getModulus());
    return pub_step;
}

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
    } catch (std::runtime_error &e){
        _isDead = true;
        std::cerr << e.what();
    }    
}

void Worker::stop(){
    _peerSocket.shutdownAndClose();
}

void Worker::newCertificate(Protocol &protocol){
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
        uint16_t hashed_cert = hasher(cert);
        uint32_t encryptedCert = _handleEncryption(hashed_cert, clientPubKey);
        _database.insert(subject, clientPubKey);
        protocol.sendByte(0);
        protocol.sendCertificate(ch);
        protocol.sendFingerPrint(encryptedCert);
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
        Key clientPubKey = _database.getPublicKey(subject);
        std::string cert = ch.createCertificate();
        Hash hasher;
        uint16_t hashed_cert = hasher(cert);
        uint32_t receivedFingerPrint = 0;
        protocol.receiveFingerPrint(&receivedFingerPrint);
        uint32_t unencryptedCert = _handleEncryption(receivedFingerPrint,
         clientPubKey);
        if (hashed_cert == unencryptedCert){
            _database.remove(subject);
            protocol.sendByte(0);
        } else {
            protocol.sendByte(2);
        }
    } else{
        protocol.sendByte(1);
    }
}
