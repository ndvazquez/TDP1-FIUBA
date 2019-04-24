#include "script_container.h"
#include <string>

ScriptContainer::ScriptContainer(): _priority(__INT_MAX__), _isValid(false){}

ScriptContainer::ScriptContainer(
        int priority,
        std::string &&input,
        std::string &&output,
        std::string &&script) :
        _priority(priority),
        _inputFile(input),
        _outputFile(output),
        _script(script),
        _isValid(true){
}

int ScriptContainer::getPriority(){
    return _priority;
}

std::string &ScriptContainer::getInputFile(){
    return _inputFile;
}
std::string &ScriptContainer::getOutputFile(){
    return _outputFile;
}
std::string &ScriptContainer::getScript(){
    return _script;
}

bool ScriptContainer::isValid(){
    return _isValid;
}
