#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <string>

class Interpreter{
    std::string _scriptPath;
    public:
    explicit Interpreter(std::string scriptPath);
    void run();
};

#endif
