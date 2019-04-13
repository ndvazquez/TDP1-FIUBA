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
    // Esto loopea hasta que el lambda devuelva true y ahí levanta el lock.
    // Debería levantar el lock cuando hay cosas en la cola y no terminé.
    // Y también cuando la cola está vacía y terminé.
    _cv.wait(_lock, [this]{return (!isEmpty() && !isFinished());});
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
    // Notifico a todos que... cambió el estado? No debería ser necesario.
    std::unique_lock<std::mutex> _lock(_mtx);
    _finished = true;
    _cv.notify_all();
}
