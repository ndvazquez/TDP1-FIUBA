#include "common_hash.h"
#include <string>

Hash::Hash(){}

uint16_t Hash::operator()(const std::string& text){
    uint16_t result = 0;
    std::string::const_iterator it = text.begin();
    for (; it != text.end(); ++it){
        result += *it;
    }
    return result;
}
