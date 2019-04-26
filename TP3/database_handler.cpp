#include "database_handler.h"
#include "key.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>

void DatabaseHandler::process_line(std::string line,
                                std::map<std::string, Key> &map){
    //TODO: Implementar usando un iterador, y no tocando line.
    std::string delimiter = "; ";
    size_t pos = line.find(delimiter);
    std::string subject = line.substr(0, pos);
    line.erase(0, pos + delimiter.length());
    pos = line.find(" ");
    std::string temp = line.substr(0, pos);
    uint8_t exponent = std::stoi(temp);
    line.erase(0, pos + 1);
    uint16_t modulus = std::stoi(line);
    map[subject] = Key(exponent, modulus);
}

DatabaseHandler::DatabaseHandler(){}

DatabaseHandler::DatabaseHandler(std::string dbPath) : _databasePath(dbPath){
    std::ifstream database;
    database.open(this->_databasePath);
    if (database >> this->_next_id){
        std::string line;
        while (getline(database, line)){
            if (line.size() == 0) continue;
            process_line(line, this->_clients);
        }
    } else {
        this->_next_id = 1;
    }
}

DatabaseHandler::~DatabaseHandler(){
    std::ofstream database;
    database.open(this->_databasePath, std::ios::out | std::ios::trunc);
    database << this->_next_id;
    for (const auto& n : this->_clients) {
        database << std::endl << n.first << "; " << n.second;
    }
}

void DatabaseHandler::insert(const std::string &subject, Key &publicKey){
    this->_clients[subject] = std::move(publicKey);
    ++this->_next_id;
}

bool DatabaseHandler::lookup(std::string &subject) const{
    return this->_clients.count(subject) > 0;
}

void DatabaseHandler::remove(const std::string &subject){
    this->_clients.erase(subject);
}

int DatabaseHandler::getNextId() const{
    return this->_next_id;
}

Key DatabaseHandler::getPublicKey(std::string &subject){
    return _clients[subject];
}
