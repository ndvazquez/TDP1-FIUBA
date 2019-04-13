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
        PriorityQueueProtected pq;
        std::vector<Thread*> threads;
        int numberOfThreads = atoi(argv[2]);
        for (int i = 0; i < numberOfThreads; ++i){
            threads.push_back(new Worker(pq));
        }
        for (int i = 0; i < numberOfThreads; ++i){
            threads[i]->start();
        }
        Tokenizer tokenizer;
        std::string line;
        while (getline(std::cin, line, ')')){
            if (!std::cin.good()) {
                continue;
                }
            std::vector<std::string> strings = tokenizer.tokenize(line);
            ScriptContainer sc(
                std::move(strings[1]),
                std::move(strings[2]),
                std::move(strings[3]),
                std::move(strings[4])); 
            pq.push(sc);
        }
        while (!pq.isFinished()){
            if (pq.isEmpty()){
                pq.finish();
                std::cout << "Terminamos de procesar, goodbye!" << std::endl;
            } 
        }
        for (int i = 0; i < numberOfThreads; ++i){
            std::cout << "Joineando el Thread " << i << std::endl;
            threads[i]->join();
            std::cout << "Termino de joinear el Thread " << i << std::endl;
            delete threads[i];
        }       
    }
}
