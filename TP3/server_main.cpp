#include "server_server.h"
#include "common_key.h"
#include "server_database_handler.h"
#include <vector>
#include <fstream>
#include <string>
#define POS_SERVICE 1
#define POS_SERVER_KEYS_FILE 2
#define POS_DATABASE_PATH 3
#define KEYS_SIZE 3
#define SERVER_PRIV_EXPONENT 1
#define SERVER_PUB_EXPONENT 0
#define SERVER_MODULUS 2
#define HOST_NAME "localhost"

int main(int argc, char** argv){
    std::ifstream keyFile;
    // Validar que se abrio bien.
    keyFile.open(argv[POS_SERVER_KEYS_FILE]);
    std::vector<std::string> keyInfo;
    keyInfo.reserve(KEYS_SIZE);
    std::string line;
    while (getline(keyFile, line, ' ')){
        keyInfo.push_back(line);
    }
    Key privateKey(std::stoi(keyInfo[SERVER_PRIV_EXPONENT]),
                    std::stoi(keyInfo[SERVER_MODULUS]));
    Key publicKey(std::stoi(keyInfo[SERVER_PUB_EXPONENT]),
                    std::stoi(keyInfo[SERVER_MODULUS]));
    std::string dbPath = argv[POS_DATABASE_PATH];
    std::string host = HOST_NAME;
    std::string service = argv[POS_SERVICE];
    
    Server server(privateKey, publicKey, dbPath);
    try {
        server.run(host, service);
    } catch (...){
        return 1;
    }
}
