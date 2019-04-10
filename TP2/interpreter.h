#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stack>
#include <string>

using namespace std;

class Interpreter{
    static const int _size = 30000;
    char _array[_size];
    char *_dp;
    string::iterator _ip;
    stack<string::iterator> _stack;

    void increaseDataPointer();
    void decreaseDataPointer();
    void increaseDataValue();
    void decreaseDataValue();
    void printDataValue();
    void readDataValue();
    void openingBracket();
    void closingBracket();

    public:
    Interpreter(string &script_buffer);
    void run();
};

#endif
