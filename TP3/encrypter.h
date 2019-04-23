#ifndef ENCRYPTER_H
#define ENCRYPTER_H
#include <cstdint>

class Encrypter{
    public:
    Encrypter();
    uint32_t rsa(uint32_t msg, uint8_t exp, uint16_t mod);
};

#endif
