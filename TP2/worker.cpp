#include "worker.h"
#include "interpreter.h"
#include "script_container.h"

Worker::Worker(PriorityQueueProtected &pq) : _pq(pq){}
void Worker::run(){
    bool keep_working = true;
    while(keep_working){    
        ScriptContainer sc = _pq.pop();
        if (sc.isValid()){
            Interpreter interpreter(sc);
            interpreter.run();
        } else {
            keep_working = false;
        }
    }   
}
