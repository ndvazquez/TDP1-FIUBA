#include "encrypter.h"
#include <cstdint>

Encrypter::Encrypter(){}

uint32_t Encrypter::rsa(uint32_t msg, 
                            uint8_t exp, 
                            uint16_t mod){
    uint32_t ret = 0;
    for (int i = 0; i < 3; ++i){
        uint32_t result = (msg >> (i * 8) & 0xff);
        uint8_t base = result;
        for (int j = 1; j < exp; ++j){
            result = (result * base) % mod;
        }
        ret += (result << (i * 8));
    }
    return ret;
}
