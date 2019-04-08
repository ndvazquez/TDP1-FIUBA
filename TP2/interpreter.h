#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <fstream>
#include <stack>
using namespace std;

class Interpreter{
    static const int _size = 30000;
    char _array[_size];
    char *_dp;
    const char *_ip;
    stack<int> _stack;

    void increaseDataPointer();
    void decreaseDataPointer();
    void increaseDataValue();
    void decreaseDataValue();
    void printDataValue();
    void readDataValue();
    void openingBracket(const char *start);
    void closingBracket(const char *start);

    public:
    Interpreter(char *script_buffer);

    void run();
};

#endif
