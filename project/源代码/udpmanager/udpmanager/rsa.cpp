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
