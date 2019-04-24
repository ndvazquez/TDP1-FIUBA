#include <cstring>
#include <string>
#include <vector>
#include "brainfuck.h"
#include "interpreter.h"
#include "tokenizer.h"
#include "worker.h"
#include "priority_queue_protected.h"
#include "threadpool.h"

#define ARGC_SIZE 3
int main(int argc, char **argv){
    if (argc < ARGC_SIZE){
        return 1;
    }
    if (strcmp(argv[1], "interprete") == 0){
        std::string scriptPath = argv[2];
        Interpreter interpreter(scriptPath);
        interpreter.run();
    }
    if (strcmp(argv[1], "thread-pool") == 0){
        int numberOfThreads = atoi(argv[2]);
        Threadpool threadpool(numberOfThreads);
        threadpool.run();  
    }
}
