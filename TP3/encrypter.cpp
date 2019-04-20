#include "encrypter.h"

Encrypter::Encrypter(){}

unsigned int Encrypter::rsa(unsigned int msg, 
                            unsigned char exp, 
                            unsigned short mod){
    unsigned int ret = 0;
    for (int i = 0; i < 3; ++i){
        unsigned int result = (msg >> (i * 8) & 0xff);
        unsigned char base = result;
        for (int j = 1; j < exp; ++j){
            result = (result * base) % mod;
        }
        ret += (result << (i * 8));
    }
    return ret;
}
