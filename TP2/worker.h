#ifndef WORKER_H
#define WORKER_H

#include "priority_queue_protected.h"
#include "thread.h"

class Worker : public Thread{
    PriorityQueueProtected &_pq;
    public:
    explicit Worker(PriorityQueueProtected &pq);
    void run();
};

#endif
