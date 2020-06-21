
#include "rsa.h"
#include <time.h>
MyRSA::MyRSA()
{

}

MyRSA::~MyRSA()
{

}

//生成字符串形式的公钥私钥
void MyRSA::GenerateRSAKey(unsigned int keyLength)
{
    AutoSeededRandomPool rng;
    InvertibleRSAFunction privkey;
    privkey.GenerateRandomWithKeySize(rng, keyLength);

    Base64Encoder privkeysink(new StringSink(privateKey));  //"privkey string"
    privkey.DEREncode(privkeysink);
    privkeysink.MessageEnd();

    RSAFunction pubkey(privkey);
    Base64Encoder pubkeysink(new StringSink(publicKey));  //"privkey string"
    pubkey.DEREncode(pubkeysink);
    pubkeysink.MessageEnd();
}

//对字符串进行加密
string MyRSA::Encrypt(const char * message)
{
//    FileSource pubFile(pubFilename, true, new Base64Decoder);
    StringSource pubKey(publicKey, true, new Base64Decoder);

    RSAES_OAEP_SHA_Encryptor pub(pubKey);
    string result;
    StringSource(message, true,
            new PK_EncryptorFilter(_rng, pub,
                    new HexEncoder(new StringSink(result))));
    return result;
}

//对字符串进行解密
string MyRSA::Decrypt(const char * ciphertext)
{
//    FileSource privFile(privFilename, true, new Base64Decoder);
    StringSource priKey(privateKey, true, new Base64Decoder);

    RSAES_OAEP_SHA_Decryptor priv(priKey);
    string result;
    StringSource(ciphertext, true,
            new HexDecoder(
                    new PK_DecryptorFilter(_rng, priv,
                            new StringSink(result))));
    return result;
}

void MyRSA::setPublicKey(string pubkey)
{
    publicKey=pubkey;
}

void MyRSA::setPrivateKey(string privkey)
{
    privateKey=privkey;
}

//对二进制内容进行加密
void MyRSA::encryptBinary(uint8_t* pPlainData, uint64_t u64PlainDataLen, uint8_t* pCipherData, uint64_t& u64CipherDataLen)
{
    StringSource pubKey(publicKey, true, new Base64Decoder);
    RSAES_OAEP_SHA_Encryptor enc(pubKey);

    uint64_t putLen = 0;
    uint64_t fixedLen = enc.FixedMaxPlaintextLength();

    for (uint64_t i = 0; i < u64PlainDataLen; i += fixedLen)
    {
        uint64_t len = fixedLen < (u64PlainDataLen - i) ? fixedLen : (u64PlainDataLen - i);
        CryptoPP::ArraySink* dstArr = new CryptoPP::ArraySink(pCipherData + putLen, (size_t)(u64CipherDataLen - putLen));
        CryptoPP::ArraySource source(pPlainData + i, (size_t)len, true, new CryptoPP::PK_EncryptorFilter(_rng, enc, dstArr));
        putLen += dstArr->TotalPutLength();
    }

    u64CipherDataLen = putLen;
}

//对二进制内容进行解密
void MyRSA::decryptBinary(uint8_t* pCipherData, uint64_t u64CipherDataLen, uint8_t* pPlainData, uint64_t& u64PlainDataLen)
{
    StringSource priKey(privateKey, true, new Base64Decoder);
    RSAES_OAEP_SHA_Decryptor dec(priKey);

    uint64_t putLen = 0;
    uint64_t fixedLen = dec.FixedCiphertextLength();

    for (uint64_t i = 0; i < u64CipherDataLen; i += fixedLen)
    {
        uint64_t len = fixedLen < (u64CipherDataLen - i) ? fixedLen : (u64CipherDataLen - i);
        CryptoPP::ArraySink* dstArr = new CryptoPP::ArraySink(pPlainData + putLen, (size_t)(u64PlainDataLen - putLen));
        CryptoPP::ArraySource source(pCipherData + i, (size_t)len, true, new CryptoPP::PK_DecryptorFilter(_rng, dec, dstArr));
        putLen += dstArr->TotalPutLength();
    }
    u64PlainDataLen = putLen;
}
