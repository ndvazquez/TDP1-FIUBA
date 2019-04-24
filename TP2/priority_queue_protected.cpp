#include "priority_queue_protected.h"
#include "script_container.h"
#include <mutex>

PriorityQueueProtected::PriorityQueueProtected(){}

void PriorityQueueProtected::push(ScriptContainer sc){
    std::unique_lock<std::mutex> _lock(_mtx);
    _pq.push(sc);
    _cv.notify_one();
}

ScriptContainer PriorityQueueProtected::pop(){
    std::unique_lock<std::mutex> _lock(_mtx);
    while (_pq.empty()){
        _cv.wait(_lock);
    }
    ScriptContainer sc = _pq.top();
    _pq.pop();
    return sc;
}
