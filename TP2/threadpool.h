#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "priority_queue_protected.h"
#include <vector>
#include "thread.h"
#include "priority_queue_protected.h"

class Threadpool{
    std::vector<Thread*> _threads;
    int _numberOfThreads;
    PriorityQueueProtected _pq;

    public:
    explicit Threadpool(int numberOfThreads);
    ~Threadpool();
    void run();
};

#endif
