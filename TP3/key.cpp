#include "key.h"
#include <iostream>

Key::Key(){}

Key::Key(unsigned char exponent, unsigned short modulus) :
                _exponent(exponent),
                _modulus(modulus){}

 std::ostream& operator<<(std::ostream& os, const Key &key){
    os << (int) key.getExponent() << " " << key.getModulus();
    return os;
}

unsigned char Key::getExponent() const {
    return _exponent;
}

unsigned short Key::getModulus() const {
    return _modulus;
}
