#include "script_container.h"
#include <string>

ScriptContainer::ScriptContainer(std::string priority,
        std::string input,
        std::string output,
        std::string script) :
        _priority(priority),
        _inputFile(input),
        _outputFile(output),
        _script(script){
}

std::string ScriptContainer::getPriority(){
    return _priority;
}
std::string ScriptContainer::getInputFile(){
    return _inputFile;
}
std::string ScriptContainer::getOutputFile(){
    return _outputFile;
}
std::string ScriptContainer::getScript(){
    return _script;
}
