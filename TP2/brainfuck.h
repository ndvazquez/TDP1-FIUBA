#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include "script_container.h"
#include <stack>
#include <string>
#include <fstream>

class Brainfuck{
    static const int _size = 30000;
    char _array[_size];
    char *_dp;
    std::string _script;
    std::string::iterator _ip;
    std::ofstream _outputStream;
    std::ifstream _inputStream;

    std::stack<std::string::iterator> _stack;

    void increaseDataPointer();
    void decreaseDataPointer();
    void increaseDataValue();
    void decreaseDataValue();
    void printDataValue();
    void readDataValue();
    void openingBracket();
    void closingBracket();

    public:
    explicit Brainfuck(std::string &&script_buffer);
    explicit Brainfuck(ScriptContainer sc);
    ~Brainfuck();
    void run();
};

#endif
