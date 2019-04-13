#include "priority_queue_protected.h"
#include "script_container.h"
#include <mutex>

PriorityQueueProtected::PriorityQueueProtected(){
    _finished = false;
}

void PriorityQueueProtected::push(ScriptContainer sc){
    // Wake up!
    std::unique_lock<std::mutex> _lock(_mtx);
    _pq.push(sc);
    _cv.notify_one();
}

ScriptContainer PriorityQueueProtected::pop(){
    std::unique_lock<std::mutex> _lock(_mtx);
    ScriptContainer sc;
    while (isEmpty() && !isFinished()){
        _cv.wait(_lock);
    }
    if (!isEmpty()){
        sc = _pq.top();
        _pq.pop();
    }
    return sc;
}

bool PriorityQueueProtected::isEmpty(){
    return _pq.empty();
}

bool PriorityQueueProtected::isFinished(){
    return _finished;
}

void PriorityQueueProtected::finish(){
    std::unique_lock<std::mutex> _lock(_mtx);
    _finished = true;
    _cv.notify_all();
}
