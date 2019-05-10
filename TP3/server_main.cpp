#include "server_server.h"
#include "common_key.h"
#include "server_database_handler.h"
#include <vector>
#include <fstream>
#include <string>

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
    
    Server server(privateKey, publicKey, dbPath);
    try {
        server.run(host, service);
    } catch (...){
        return 1;
    }
}
