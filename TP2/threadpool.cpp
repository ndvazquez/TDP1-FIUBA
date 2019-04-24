#include "threadpool.h"
#include "worker.h"
#include "tokenizer.h"
#include "priority_queue_protected.h"
#include <vector>
#include <iostream>

Threadpool::Threadpool(int numberOfThreads):_numberOfThreads(numberOfThreads){
    this->_threads.reserve(_numberOfThreads);
    for (int i = 0; i < _numberOfThreads; ++i){
        this->_threads.push_back(new Worker(_pq));
    }
}

Threadpool::~Threadpool(){
    for (size_t i = 0; i < this->_threads.size(); ++i){
        this->_threads[i]->join();
        delete this->_threads[i];
    }
}

void Threadpool::run(){
    for (size_t i = 0; i < this->_threads.size(); ++i){
        this->_threads[i]->start();
    }
    Tokenizer tokenizer;
    std::string line;
    while (getline(std::cin, line, ')')){
        if (!std::cin.good()) {
            continue;
            }
        std::vector<std::string> strings = tokenizer.tokenize(line);
        ScriptContainer sc(
            std::stoi(strings[1], nullptr, 10),
            std::move(strings[2]),
            std::move(strings[3]),
            std::move(strings[4])); 
        _pq.push(sc);
    }
    for (int i = 0; i < _numberOfThreads; ++i){
        ScriptContainer invalid_sc;
        _pq.push(invalid_sc);
    }
}
