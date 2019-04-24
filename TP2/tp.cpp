#include <queue> 
#include <sstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
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
        int file_size;
        std::ifstream file;
        file.open(argv[2]);
        file.seekg(0, file.end);
        file_size = file.tellg();
        file.seekg(0, file.beg);

        std::string buffer(file_size, '\0');
        file.read(&buffer[0], file_size);
        file.close();
        Interpreter interpreter(std::move(buffer));
        interpreter.run();
    }
    if (strcmp(argv[1], "thread-pool") == 0){
        int numberOfThreads = atoi(argv[2]);
        Threadpool threadpool(numberOfThreads);
        threadpool.run();  
    }
}
