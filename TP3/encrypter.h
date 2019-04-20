#ifndef ENCRYPTER_H
#define ENCRYPTER_H

class Encrypter{
    public:
    Encrypter();
    unsigned int rsa(unsigned int msg, unsigned char exp, unsigned short mod);
};

#endif
