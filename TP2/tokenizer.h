#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <string>

class Tokenizer{
    public:
    Tokenizer();
    std::vector<std::string> tokenize(std::string line);
};

#endif