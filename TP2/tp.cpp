#include "interpreter.h"
#include <iostream>
int main(int argc, char **argv){
    char c;
    int file_size, pos = 0;
    ifstream file;

    file.open(argv[1]);
    file.seekg(0, file.end);
    file_size = file.tellg();
    file.seekg(0, file.beg);

    char *script_buffer = new char[file_size+1];
    //string script_buffer(file_size+1, '\0');
    while (file.get(c)){
        if (c == '\n') continue;
        //script_buffer.push_back(c);
        script_buffer[pos++] = c;
    }
    script_buffer[pos] = '\0';
    file.close();

    Interpreter interpreter(script_buffer);
    interpreter.run();
    
    delete[] script_buffer;
}

