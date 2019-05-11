#include "server_acceptor.h"
#include "server_worker.h"
#include "server_thread.h"
#include <algorithm>

Acceptor::Acceptor(DatabaseHandler &db, Socket &socket, Key &key):
                _database(db),
                _socket(socket),
                _serverPrivateKey(key),
                _keepRunning(true){}

bool isDone(Worker* worker){
    return worker->isDead();
}

void Acceptor::run(){
    while (_keepRunning){
        Socket peerSocket = _socket.acceptPeer();
        if (!peerSocket.isValid()) continue;
        _workers.push_front(new Worker(_database, 
                                    peerSocket, 
                                    _serverPrivateKey));
        _workers.front()->start();
        auto it = _workers.begin();
        while (it != _workers.end()){
            if ((*it)->isDead()){
                (*it)->join();
                delete *it;
                it = _workers.erase(it);
            } else{
                ++it;
            }
        }
    }
    for (auto it = _workers.begin(); it != _workers.end(); ++it){
        (*it)->stop();
        (*it)->join();
        delete *it;
    }
}

void Acceptor::stop(){
    _keepRunning = false;
    _socket.shutdownAndClose();
}
