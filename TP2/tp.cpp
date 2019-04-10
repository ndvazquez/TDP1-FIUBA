#include "interpreter.h"
#include <fstream>
#include <iostream>
#include <cstring>

#define ARGC_SIZE 3
int main(int argc, char **argv){
    if (argc < ARGC_SIZE){
        return 1;
    }

    int file_size;
    ifstream file;
    if (strcmp(argv[1], "interprete") == 0){
        file.open(argv[2]);
        file.seekg(0, file.end);
        file_size = file.tellg();
        file.seekg(0, file.beg);

        std::string buffer(file_size, '\0');
        file.read(&buffer[0], file_size);
        file.close();
        Interpreter interpreter(buffer);
        interpreter.run();
    }
}
