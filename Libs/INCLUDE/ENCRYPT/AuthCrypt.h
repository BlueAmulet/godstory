#ifndef _AUTHCRYPT_H
#define _AUTHCRYPT_H

#include <vector>

typedef unsigned char uint8;
class BigNumber;

#define SHA_DIGEST_LENGTH 20

/************************************************************************/
/* 轻量级的字符加密和解密
/************************************************************************/
class AuthCrypt
{
public:
    AuthCrypt();

    static AuthCrypt* Instance(void)
    {
        static AuthCrypt local;
        return &local;
    }
    
    void SetKey(BigNumber*);

    void DecryptRecv(uint8 *, size_t);
    void EncryptSend(uint8 *, size_t);
private:
    void __GenerateKey(uint8 *, BigNumber *);

    uint8 _key[SHA_DIGEST_LENGTH];
    uint8 _send_i, _send_j, _recv_i, _recv_j;
};

#endif
