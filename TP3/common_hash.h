#ifndef HASH_H
#define HASH_H
#include <string>

class Hash{
    public:
    Hash();
    uint16_t operator() (const std::string& text);
};

#endif
