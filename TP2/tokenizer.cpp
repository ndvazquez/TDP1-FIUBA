#include "tokenizer.h"
#include <vector>
#include <string>

Tokenizer::Tokenizer(){}

std::vector<std::string> Tokenizer::tokenize(std::string line){
    std::vector<std::string> strings;
    std::string delimiter = ", ";
    size_t pos = 0;
    std::string token;
    strings.reserve(5);
    while ((pos = line.find(delimiter)) != std::string::npos){
        token = line.substr(0, pos);
        strings.push_back(std::move(token));
        line.erase(0, pos + delimiter.length());
    }
    strings.push_back(line);
    return strings;
}
