/*
 * MyRSA.h
 *
 *  Created on: 2013-3-7
 *      Author: wzb
 */

#ifndef MYRSA_H_
#define MYRSA_H_

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <iostream>
#include <string>

#include <cryptopp/rsa.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>

#include <cryptopp/randpool.h>
#include <cryptopp/base64.h>

using namespace std;
using namespace CryptoPP;

class MyRSA
{
public:
    MyRSA();
    ~MyRSA();

    void GenerateRSAKey(unsigned int keyLength);

    string Encrypt(const char * message);
    string Decrypt(const char * cipher);

    string publicKey;
    string privateKey;

private:
    AutoSeededRandomPool _rng;
};

#endif /* MYRSA_H_ */
