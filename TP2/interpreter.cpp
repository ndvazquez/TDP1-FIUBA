#include "interpreter.h"
#include <cstring>
#include <iostream>

Interpreter::Interpreter(char *script_buffer){
    for(int i = 0; i < this->_size; ++i){
        this->_array[i] = 0;
    }
    this->_dp = this->_array;
    this->_ip = script_buffer;
}

void Interpreter::increaseDataPointer(){
    _dp += 1;
}

void Interpreter::decreaseDataPointer(){
    _dp -= 1;
}

void Interpreter::increaseDataValue(){
    *_dp += 1;
}

void Interpreter::decreaseDataValue(){
    *_dp -= 1;
}

void Interpreter::printDataValue(){
    cout << *_dp;
}

void Interpreter::readDataValue(){
    char c;
    cin.get(c);
    if (cin.eof()) {
        *_dp = 0;
    } else {
        *_dp = c;
    }
}

void Interpreter::openingBracket(const char *start){
    if (*_dp){
        _stack.push(_ip - start);
    } else {
        int counter = 1;
        while (counter){
            _ip++;
            if (*_ip == '[') counter++;
            else if (*_ip == ']') counter--;
        }
    }
}

void Interpreter::closingBracket(const char *start){;
    if (*_dp){
        _ip = start + _stack.top();
    } else {
        _stack.pop();
    }
}

void Interpreter::run(){
    const char *start = _ip;
    while (*_ip){
        switch (*_ip){
            case '>':
                increaseDataPointer();
                break;
            case '<':
                decreaseDataPointer();
                break;
            case '+':
                increaseDataValue();
                break;
            case '-':
                decreaseDataValue();
                break;
            case ',':
                readDataValue();
                break;
            case '.':
                printDataValue();
                break;
            case '[':
                openingBracket(start);
                break;
            case ']':
                closingBracket(start);
                break;
            default:
                break;
        }
        _ip++;
    }
}

