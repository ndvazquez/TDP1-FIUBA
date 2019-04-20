#ifndef HASH_H
#define HASH_H
#include <string>

class Hash{
    public:
    Hash();
    unsigned short operator() (const std::string& text);
};

#endif
