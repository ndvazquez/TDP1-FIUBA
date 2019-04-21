#ifndef KEY_H
#define KEY_H
#include <iostream>

class Key{
    unsigned char _exponent;
    unsigned short _modulus;
    public:
    Key();
    Key(unsigned char exponent, unsigned short modulus);
    friend std::ostream& operator<<(std::ostream &os, const Key &key);
    unsigned char getExponent() const;
    unsigned short getModulus() const;

};

#endif
