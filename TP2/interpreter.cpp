#include "interpreter.h"
#include <iostream>
#include <cstring>
#include <string>

Interpreter::Interpreter(std::string &&script_buffer){
    memset(this->_array, 0, this->_size);
    this->_dp = this->_array;
    this->_script = script_buffer;
    this->_ip = _script.begin();
}

Interpreter::Interpreter(ScriptContainer sc){
    memset(this->_array, 0, this->_size);
    this->_dp = this->_array;
    this->_script = sc.getScript();
    this->_ip = _script.begin();
    this->_outputStream.open(sc.getOutputFile());
    this->_inputStream.open(sc.getInputFile());
}

Interpreter::~Interpreter(){
    if (this->_outputStream.is_open()){
        this->_outputStream.close();
    }
    if (this->_inputStream.is_open()){
        this->_inputStream.close();
    }
}

void Interpreter::increaseDataPointer(){
    if (_dp == _array+_size) return;   
    _dp += 1;
}

void Interpreter::decreaseDataPointer(){
    if (_dp == _array) return; 
    _dp -= 1;
}

void Interpreter::increaseDataValue(){
    *_dp += 1;
}

void Interpreter::decreaseDataValue(){
    *_dp -= 1;
}

void Interpreter::printDataValue(){
    if (_outputStream.is_open()){
        _outputStream << *_dp;
    } else{
        std::cout << *_dp;
    }
}

void Interpreter::readDataValue(){
    char c;
    std::istream& is = _inputStream.is_open() ? _inputStream : std::cin;
    is.get(c);
    if (is.eof()) {
        *_dp = 0;
    } else {
        *_dp = c;
    }
}

void Interpreter::openingBracket(){
    if (*_dp){
        _stack.push(_ip);
    } else {
        int counter = 1;
        while (counter){
            _ip++;
            if (*_ip == '[') counter++;
            else if (*_ip == ']') counter--;
        }
    }
}

void Interpreter::closingBracket(){;
    if (*_dp){
        _ip = _stack.top();
    } else {
        _stack.pop();
    }
}

void Interpreter::run(){    
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
                openingBracket();
                break;
            case ']':
                closingBracket();
                break;
            default:
                break;
        }
        _ip++;
    }
}
