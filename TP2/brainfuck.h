#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include "script_container.h"
#include <stack>
#include <string>
#include <vector>
#include <fstream>

class Brainfuck{
    //This implementation uses a fixed size data array of 30000 elements.
    static const int _size = 30000;
    char _array[_size];
    char *_dp;
    std::vector<char> _script;
    std::vector<char>::iterator _ip;
    std::ofstream _outputStream;
    std::ifstream _inputStream;

    std::stack<std::vector<char>::iterator> _stack;

    void increaseDataPointer();
    void decreaseDataPointer();
    void increaseDataValue();
    void decreaseDataValue();
    void printDataValue();
    void readDataValue();
    void openingBracket();
    void closingBracket();

    public:
    explicit Brainfuck(std::vector<char> &&script_buffer);
    explicit Brainfuck(ScriptContainer sc);
    ~Brainfuck();
    void run();
};

#endif
