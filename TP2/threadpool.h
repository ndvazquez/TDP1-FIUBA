#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include "thread.h"
#include "priority_queue_protected.h"

class Threadpool{
    std::vector<Thread*> _threads;

    public:
    Threadpool(int numberOfThreads, PriorityQueueProtected &pq);
    ~Threadpool();
    void run();
};

#endif
