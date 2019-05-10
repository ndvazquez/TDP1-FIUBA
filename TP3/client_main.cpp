#include "client_client.h"
#include "common_socket.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

int main(int argc, char** argv){
    std::string mode = argv[3];
    if (argc != 7 || (mode.compare(MODE_NEW) && mode.compare(MODE_REVOKE))){
        std::cout << "Error: argumentos invalidos.\n";
    }
    std::string host = argv[1];
    std::string port = argv[2];
    std::string clientKeysPath = argv[5];
    std::string serverKeysPath = argv[6];
    std::string filePath = argv[4];
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
    keyInfo.reserve(3);
    std::string line;
    while (getline(keyFile, line, ' ')){
        keyInfo.push_back(line);
    }
    Key privateKey(std::stoi(keyInfo[1]), std::stoi(keyInfo[2]));
    Key publicKey(std::stoi(keyInfo[0]), std::stoi(keyInfo[2]));
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
    keyInfo.reserve(2);
    while (getline(serverKeyFile, line, ' ')){
        keyInfo.push_back(line);
    }
    Key serverPubKey(std::stoi(keyInfo[0]), std::stoi(keyInfo[1]));
    Socket clientSocket;
    int status = clientSocket.connectToHost(host, port);
    if (status == -1) return 0;
    Client client(privateKey, publicKey, serverPubKey, clientSocket, filePath);

    if (!mode.compare(MODE_NEW)) client.requestCertificateCreation();
    if (!mode.compare(MODE_REVOKE)) client.requestCertificateRevocation();
}
