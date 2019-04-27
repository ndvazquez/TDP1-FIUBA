#include "acceptor.h"
#include "worker.h"

Acceptor::Acceptor(DatabaseHandler &db, Socket &socket, Key &key):
                _database(db),
                _socket(socket),
                _serverPrivateKey(key),
                _keepRunning(true){}

void Acceptor::run(){
    while (_keepRunning){
        Socket peerSocket = _socket.acceptPeer();
        if (!peerSocket.isValid()) continue;
        _workers.push_back(new Worker(_database, peerSocket, _serverPrivateKey));
        _workers.back()->start();
        for (size_t i = 0; i < _workers.size(); ++i){
            if (_workers[i]->isDead()){
                _workers[i]->join();
                delete _workers[i];
                _workers.erase(_workers.begin() + i);
            }
        }
    }
    for (size_t i = 0; i < _workers.size(); ++i){
        _workers[i]->stop();
        _workers[i]->join();
        delete _workers[i];
    }
}

void Acceptor::stop(){
    _keepRunning = false;
    _socket.shutdownAndClose();
}
