#include "threadpool.h"
#include "worker.h"
#include "priority_queue_protected.h"
#include <vector>

Threadpool::Threadpool(int numberOfThreads, PriorityQueueProtected &pq){
    this->_threads.reserve(numberOfThreads);
    for (int i = 0; i < numberOfThreads; ++i){
        this->_threads.push_back(new Worker(pq));
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
}
