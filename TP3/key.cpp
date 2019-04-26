#include "key.h"
#include <iostream>

Key::Key(){}

Key::Key(Key&& other){
    *this = std::move(other);
}

Key& Key::operator=(Key&& other){
    if (this == &other){
        return *this;
    }
    this->_exponent = other._exponent;
    this->_modulus = other._modulus;

    other._modulus = 0;
    other._exponent = 0;
    return *this;
}
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
