#include "server_database_handler.h"
#include "common_key.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <mutex>

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

DatabaseHandler::~DatabaseHandler(){
    std::ofstream database;
    database.open(this->_databasePath, std::ios::out | std::ios::trunc);
    database << this->_next_id;
    for (const auto& n : this->_clients) {
        database << std::endl << n.first << "; " << n.second;
    }
    database << std::endl;
}

void DatabaseHandler::initializeData(std::string dbPath){
    this->_databasePath = dbPath;
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
void DatabaseHandler::insert(const std::string &subject, Key &publicKey){
    std::unique_lock<std::mutex> _lock(_mtx);
    this->_clients[subject] = std::move(publicKey);
    ++this->_next_id;
}

bool DatabaseHandler::lookup(std::string &subject){
    std::unique_lock<std::mutex> _lock(_mtx);
    return this->_clients.count(subject) > 0;
}

void DatabaseHandler::remove(const std::string &subject){
    std::unique_lock<std::mutex> _lock(_mtx);
    this->_clients.erase(subject);
}

int DatabaseHandler::getNextId(){
    std::unique_lock<std::mutex> _lock(_mtx);
    return this->_next_id;
}

Key DatabaseHandler::getPublicKey(std::string &subject){
    std::unique_lock<std::mutex> _lock(_mtx);
    return _clients[subject];
}
