#include "interpreter.h"
#include "brainfuck.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

Interpreter::Interpreter(std::string scriptPath) : _scriptPath(scriptPath){}

void Interpreter::run(){
    int file_size;
    std::ifstream file;
    file.open(_scriptPath);
    if (!file.is_open()){
        std::cerr << "Couldn't open the file\n";
        return;
    }
    file.seekg(0, file.end);
    file_size = file.tellg();
    file.seekg(0, file.beg);

    std::vector<char> buffer(file_size, '\0');
    file.read(&buffer[0], file_size);
    file.close();
    Brainfuck brainfuck(std::move(buffer));
    brainfuck.run();
}
