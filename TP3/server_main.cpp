#include "server.h"
#include "key.h"
#include "database_handler.h"
#include <vector>
#include <fstream>

int main(int argc, char** argv){
    std::ifstream keyFile;
    // Validar que se abrio bien.
    keyFile.open(argv[2]);
    std::vector<std::string> keyInfo;
    keyInfo.reserve(3);
    std::string line;
    while (getline(keyFile, line, ' ')){
        keyInfo.push_back(line);
    }
    Key privateKey(std::stoi(keyInfo[1]), std::stoi(keyInfo[2]));
    Key publicKey(std::stoi(keyInfo[0]), std::stoi(keyInfo[2]));
    std::string dbPath = argv[3];
    std::string host = "localhost";
    std::string service = argv[1];
    Socket acceptor;
    int status = acceptor.bindAndListen(host, service);
    if (status == -1){
        std::cout << "No se pudo bindear al socket\n";
        return -1;
    }
    Server server(privateKey, publicKey, dbPath, acceptor);
    server.run();
}