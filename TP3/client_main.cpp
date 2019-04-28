#include "client_client.h"
#include "common_socket.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define PARAMS_SIZE 7
#define MODE_NEW "new"
#define MODE_REVOKE "revoke"

int main(int argc, char** argv){
    std::string mode = argv[3];
    if (argc != 7 || (mode.compare(MODE_NEW) && mode.compare(MODE_REVOKE))){
        std::cout << "Error: argumentos invalidos.\n";
    }
    std::ifstream keyFile;
    // Validar que se abrio bien.
    try{
        keyFile.open(argv[4]);
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
    serverKeyFile.open(argv[5]);
        if (!serverKeyFile.is_open()) throw std::ifstream::failure("File not found");
    } catch(std::ifstream::failure &e){
        std::cerr << e.what() << std::endl;
        return 0;
    }
    keyInfo.reserve(2);
    while (getline(serverKeyFile, line, ' ')){
        keyInfo.push_back(line);
    }
    Key serverPubKey(std::stoi(keyInfo[0]), std::stoi(keyInfo[1]));
    std::string host = argv[1];
    std::string port = argv[2];
    Socket clientSocket;
    int status = clientSocket.connectToHost(host, port);
    if (status == -1) return 0;
    std::string filePath = argv[6];
    Client client(privateKey, publicKey, serverPubKey, clientSocket, filePath);

    if (!mode.compare(MODE_NEW)) client.requestCertificateCreation();

}