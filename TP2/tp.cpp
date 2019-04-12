#include "interpreter.h"
#include <queue> 
#include <sstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

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
        std::priority_queue<ScriptContainer,
            std::vector<ScriptContainer>, scCompare> pq;
        std::string line;
        while (getline(std::cin, line, ')')){
            if (!std::cin.good()) continue;
            std::vector<std::string> strings;
            // Abstraer esto en un split o tokenizer propio.
            // Pensar cómo ser menos choto en memoria y
            // si realmente necesito ScriptContainer.
            std::string delimiter = ", ";
            size_t pos = 0;
            std::string token;
            while ((pos = line.find(delimiter)) != std::string::npos){
                token = line.substr(0, pos);
                strings.push_back(token);
                line.erase(0, pos + delimiter.length());
            }
            strings.push_back(line);
            ScriptContainer sc(
                strings[1],
                strings[2],
                strings[3],
                strings[4]); 
            pq.push(std::move(sc));
        }
        while (!pq.empty()){
            // Por el poder de la no concurrencia!
            ScriptContainer sc = pq.top();
            Interpreter interpreter(sc);
            interpreter.run();
            pq.pop();
        }
    }
}
