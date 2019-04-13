#ifndef PRIORITY_QUEUE_PROTECTED_H
#define PRIORITY_QUEUE_PROTECTED_H
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "script_container.h"

class PriorityQueueProtected{
    std::priority_queue<ScriptContainer,
        std::vector<ScriptContainer>, scCompare> _pq;
    std::condition_variable _cv;
    std::mutex _mtx;
    bool _finished;

    public:
    PriorityQueueProtected();
    ScriptContainer pop();
    void push(ScriptContainer sc);
    bool isEmpty();
    bool isFinished();
    void finish();
};

#endif