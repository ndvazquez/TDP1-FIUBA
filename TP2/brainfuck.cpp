#include "brainfuck.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

Brainfuck::Brainfuck(std::vector<char> &&script_buffer){
    memset(this->_array, 0, this->_size);
    this->_dp = this->_array;
    this->_script = script_buffer;
    this->_ip = _script.begin();
}

Brainfuck::Brainfuck(ScriptContainer sc){
    memset(this->_array, 0, this->_size);
    this->_dp = this->_array;
    std::string script = sc.getScript();
    std::vector<char> vScript(script.begin(), script.end());
    this->_script = std::move(vScript);
    this->_ip = _script.begin();
    this->_outputStream.open(sc.getOutputFile());
    this->_inputStream.open(sc.getInputFile());
}

Brainfuck::~Brainfuck(){
    if (this->_outputStream.is_open()){
        this->_outputStream.close();
    }
    if (this->_inputStream.is_open()){
        this->_inputStream.close();
    }
}

void Brainfuck::increaseDataPointer(){
    if (_dp == _array+_size) return;   
    _dp += 1;
}

void Brainfuck::decreaseDataPointer(){
    if (_dp == _array) return; 
    _dp -= 1;
}

void Brainfuck::increaseDataValue(){
    *_dp += 1;
}

void Brainfuck::decreaseDataValue(){
    *_dp -= 1;
}

void Brainfuck::printDataValue(){
    if (_outputStream.is_open()){
        _outputStream << *_dp;
    } else{
        std::cout << *_dp;
    }
}

void Brainfuck::readDataValue(){
    char c;
    std::istream& is = _inputStream.is_open() ? _inputStream : std::cin;
    is.get(c);
    if (is.eof()) {
        *_dp = 0;
    } else {
        *_dp = c;
    }
}

void Brainfuck::openingBracket(){
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

void Brainfuck::closingBracket(){;
    if (*_dp){
        _ip = _stack.top();
    } else {
        _stack.pop();
    }
}

void Brainfuck::run(){    
    while (_ip != _script.end()){
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
        ++_ip;
    }
}
