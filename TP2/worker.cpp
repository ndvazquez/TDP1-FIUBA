#include "worker.h"
#include "interpreter.h"
#include "script_container.h"
#include <chrono>

Worker::Worker(PriorityQueueProtected &pq) : _pq(pq){}
void Worker::run(){
    // Intento correr mientras haya que trabajar.
    while (!_pq.isFinished()){
        ScriptContainer sc = _pq.pop();
        if (sc.isValid()){
            Interpreter interpreter(sc);
            interpreter.run();
        }
    }   
}
