#include "key.h"
#include <iostream>

Key::Key(){}

Key::Key(uint8_t exponent, uint16_t modulus) :
                _exponent(exponent),
                _modulus(modulus){}

std::ostream& operator<<(std::ostream& os, const Key &key){
    os << (int) key.getExponent() << " " << key.getModulus();
    return os;
}

uint8_t Key::getExponent() const {
    return _exponent;
}

uint16_t Key::getModulus() const {
    return _modulus;
}
