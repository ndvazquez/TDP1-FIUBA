#include "hash.h"
#include <string>

Hash::Hash(){}

unsigned short Hash::operator()(const std::string& text){
    unsigned short result = 0;
    std::string::const_iterator it = text.begin();
    for (; it != text.end(); ++it){
        result += *it;
    }
    return result;
}
