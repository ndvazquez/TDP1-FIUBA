#ifndef KEY_H
#define KEY_H
#include <iostream>

class Key{
    uint8_t _exponent;
    uint16_t _modulus;
    public:
    Key();
    Key(const Key &) =default;
    Key(Key&& other);
    Key& operator=(Key&& other);
    Key(uint8_t exponent, uint16_t modulus);
    friend std::ostream& operator<<(std::ostream &os, const Key &key);
    uint8_t getExponent() const;
    uint16_t getModulus() const;
};

#endif
