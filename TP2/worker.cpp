#include "worker.h"
#include "brainfuck.h"
#include "script_container.h"

Worker::Worker(PriorityQueueProtected &pq) : _pq(pq){}
void Worker::run(){
    bool keep_working = true;
    while (keep_working){    
        ScriptContainer sc = _pq.pop();
        if (sc.isValid()){
            Brainfuck brainfuck(sc);
            brainfuck.run();
        } else {
            keep_working = false;
        }
    }   
}
