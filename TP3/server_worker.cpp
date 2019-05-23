#include "server_worker.h"
#include "server_database_handler.h"
#include "common_socket.h"
#include "common_hash.h"
#include "common_encrypter.h"
#include "common_protocol.h"
#include "common_certificate_handler.h"
#include <iostream>
#include <stdexcept>
#include <string>

Worker::Worker(DatabaseHandler &db, Socket &socket, Key &key):
                _database(db),
                _peerSocket(std::move(socket)),
                _serverPrivateKey(key){}

uint32_t Worker::_handleEncryption(uint32_t hashed_value,
        uint8_t exponent, uint16_t modulus){
    Encrypter encrypter;
    uint32_t priv_step = encrypter.rsa(hashed_value,
                                            _serverPrivateKey.getExponent(),
                                            _serverPrivateKey.getModulus());
    uint32_t pub_step = encrypter.rsa(priv_step,
                                        exponent,
                                        modulus);
    return pub_step;
}

void Worker::run(){
    try{
        Protocol protocol(_peerSocket);
        uint8_t action;
        protocol >> action;
        if (action == REQUEST_NEW_CERT){
            newCertificate(protocol);
        } else if (action == REQUEST_REVOKE_CERT){
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
    // We receive all the required data to create a cert.
    std::string subject;
    protocol >> subject;
    uint16_t modulus;
    protocol >> modulus;
    uint8_t exponent;
    protocol >> exponent;
    std::string s_date;
    protocol >> s_date;
    std::string e_date;
    protocol >> e_date;

    if (!_database.lookup(subject)){
        uint32_t serial = _database.getNextId();
        std::string issuer = DEFAULT_CERT_ISSUER;
        Key clientPubKey(exponent, modulus);
        CertificateHandler ch(serial, subject, issuer, s_date, e_date,
                            clientPubKey);
        std::string cert = ch.createCertificate();
        Hash hasher;
        uint16_t hashed_cert = hasher(cert);
        uint32_t encryptedCert = _handleEncryption(hashed_cert,
                                                exponent,
                                                modulus);
        // Lost ownership of clientPubKey so I have to create it again.
        Key key(exponent, modulus);
        _database.insert(subject, key);
        uint8_t certificateCreated = CERT_CREATION_SUCCESS;
        protocol << certificateCreated;
        protocol << ch;
        protocol << encryptedCert;
        uint8_t clientResponse;
        protocol >> clientResponse;
        if (clientResponse == CLIENT_INVALID_HASH) _database.remove(subject);
    } else{
        uint8_t subjectAlreadyExists = SUBJECT_ALREADY_EXISTS;
        protocol << subjectAlreadyExists;
    }
}

void Worker::revoke(Protocol &protocol){
    CertificateHandler ch;
    protocol >> ch;
    std::string subject = ch.getSubject();
    if (_database.lookup(subject)){
        Key clientPubKey = _database.getPublicKey(subject);
        std::string cert = ch.createCertificate();
        Hash hasher;
        uint16_t hashed_cert = hasher(cert);
        uint32_t receivedFingerPrint;
        protocol >> receivedFingerPrint;
        uint32_t unencryptedCert = _handleEncryption(receivedFingerPrint,
                                                    clientPubKey.getExponent(),
                                                    clientPubKey.getModulus());
        if (hashed_cert == unencryptedCert){
            _database.remove(subject);
            uint8_t ok = CERT_REVOCATION_SUCCESS;
            protocol << ok;
        } else {
            uint8_t invalidHash = INVALID_HASH;
            protocol << invalidHash;
        }
    } else{
        uint8_t subjectDoesntExists = SUBJECT_DOESNT_EXISTS;
        protocol << subjectDoesntExists;
    }
}
