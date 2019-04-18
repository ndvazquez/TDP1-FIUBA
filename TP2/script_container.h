#ifndef SCRIPT_CONTAINER_H
#define SCRIPT_CONTAINER_H

#include <iostream>
#include <string>

class ScriptContainer{
    std::string _priority;
    std::string _inputFile;
    std::string _outputFile;
    std::string _script;
    bool _isValid;

    public:
    ScriptContainer();
    ScriptContainer(std::string &&priority,
                    std::string &&input,
                    std::string &&output,
                    std::string &&script);
    std::string &getPriority();
    std::string &getInputFile();
    std::string &getOutputFile();
    std::string &getScript(); 
    bool isValid();
    bool compare(ScriptContainer sc);
};

struct scCompare{
    bool operator()(ScriptContainer sc1, ScriptContainer sc2){
        return sc1.getPriority().compare(sc2.getPriority()) >= 0;
    }
};

#endif
