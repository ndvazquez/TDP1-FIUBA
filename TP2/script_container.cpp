#include "script_container.h"
#include <string>

ScriptContainer::ScriptContainer(): _isValid(false){}

ScriptContainer::ScriptContainer(
        std::string &&priority,
        std::string &&input,
        std::string &&output,
        std::string &&script) :
        _priority(priority),
        _inputFile(input),
        _outputFile(output),
        _script(script),
        _isValid(true){
}

std::string &ScriptContainer::getPriority(){
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
