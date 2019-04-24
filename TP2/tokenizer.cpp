#include "tokenizer.h"
#include <vector>
#include <string>

Tokenizer::Tokenizer(){}

std::vector<std::string> Tokenizer::tokenize(std::string line){
    std::vector<std::string> strings;
    size_t tokenLength = 0;
    size_t pos = 0;
    std::string token;
    strings.reserve(5);
    std::string::iterator it;
    for (it = line.begin(); it != line.end(); ++it){
        if (*it == ',' && *(it+1) == ' '){
            token = line.substr(pos+1, tokenLength-1);
            strings.push_back(token);
            pos += tokenLength + 1;
            tokenLength = 0;
        } else{
            ++tokenLength;
        }
    }
    token = line.substr(pos+1, tokenLength-1);
    strings.push_back(token);
    return strings;
}
