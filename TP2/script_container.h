#ifndef SCRIPT_CONTAINER_H
#define SCRIPT_CONTAINER_H

#include <iostream>
#include <string>

class ScriptContainer{
    int _priority;
    std::string _inputFile;
    std::string _outputFile;
    std::string _script;
    bool _isValid;

    public:
    ScriptContainer();
    ScriptContainer(int priority,
                    std::string &&input,
                    std::string &&output,
                    std::string &&script);
    int getPriority();
    std::string &getInputFile();
    std::string &getOutputFile();
    std::string &getScript(); 
    bool isValid();
};

struct scCompare{
    bool operator()(ScriptContainer sc1, ScriptContainer sc2){
        return sc1.getPriority() >= sc2.getPriority();
    }
};

#endif
