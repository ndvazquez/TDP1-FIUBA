#include "client_client.h"
#include "common_socket.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#define POS_HOST 1
#define POS_PORT 2
#define POS_MODE 3
#define POS_REQS 4
#define POS_CLIENT_KEYS 5
#define POS_SERVER_PUB_KEYS 6
#define ARG_SIZE 7
#define CLIENT_KEYS_SIZE 3
#define SERVER_KEYS_SIZE 2
#define CLIENT_PUB_EXPONENT 0
#define CLIENT_PRIV_EXPONENT 1
#define CLIENT_MODULUS 2
#define SERVER_MODULUS 1
#define SERVER_PUB_EXPONENT 0

int main(int argc, char** argv){
    std::string mode = argv[POS_MODE];
    if (argc != ARG_SIZE || (mode.compare(MODE_NEW) &&
                            mode.compare(MODE_REVOKE))){
        std::cout << "Error: argumentos invalidos.\n";
    }
    std::string host = argv[POS_HOST];
    std::string port = argv[POS_PORT];
    std::string clientKeysPath = argv[POS_CLIENT_KEYS];
    std::string serverKeysPath = argv[POS_SERVER_PUB_KEYS];
    std::string filePath = argv[POS_REQS];
    std::ifstream keyFile;
    // Validar que se abrio bien.
    try{
        keyFile.open(clientKeysPath);
        if (!keyFile.is_open()) throw std::ifstream::failure("File not found");
    } catch(std::ifstream::failure &e){
        std::cerr << e.what() << std::endl;
        return 0;
    }
    std::vector<std::string> keyInfo;
    keyInfo.reserve(CLIENT_KEYS_SIZE);
    std::string line;
    while (getline(keyFile, line, ' ')){
        keyInfo.push_back(line);
    }
    Key privateKey(std::stoi(keyInfo[CLIENT_PRIV_EXPONENT]),
                    std::stoi(keyInfo[CLIENT_MODULUS]));
    Key publicKey(std::stoi(keyInfo[CLIENT_PUB_EXPONENT]),
                    std::stoi(keyInfo[CLIENT_MODULUS]));

    keyInfo.clear();

    std::ifstream serverKeyFile;
    try{
    serverKeyFile.open(serverKeysPath);
        if (!serverKeyFile.is_open()){
            throw std::ifstream::failure("File not found");
        }
    } catch(std::ifstream::failure &e){
        std::cerr << e.what() << std::endl;
        return 0;
    }
    keyInfo.reserve(SERVER_KEYS_SIZE);
    while (getline(serverKeyFile, line, ' ')){
        keyInfo.push_back(line);
    }

    Key serverPubKey(std::stoi(keyInfo[SERVER_PUB_EXPONENT]),
                        std::stoi(keyInfo[SERVER_MODULUS]));
    Client client(privateKey, publicKey, serverPubKey, filePath, host, port);

    if (!mode.compare(MODE_NEW)) client.requestCertificateCreation();
    if (!mode.compare(MODE_REVOKE)) client.requestCertificateRevocation();

    return 0;
}
