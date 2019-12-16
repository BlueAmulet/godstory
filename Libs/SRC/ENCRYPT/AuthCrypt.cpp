#include "AuthCrypt.h"
//#include "Sha1.h"

//这里定义1个随机的数字用来更加混乱加密后的内容
#define INC_NUM 2

AuthCrypt::AuthCrypt()
{
    _send_i = _send_j = _recv_i = _recv_j = 0;
}

void AuthCrypt::DecryptRecv(uint8 *data, size_t len)
{
    _recv_i = _recv_j = 0;

    for (size_t t = 0; t < len; ++t)
    {
        _recv_i %= SHA_DIGEST_LENGTH;
        uint8 x = (data[t] - _recv_j) ^ _key[_recv_i];
        _recv_i += INC_NUM;
        _recv_j = data[t];
        data[t] = x;
    }
}

void AuthCrypt::EncryptSend(uint8 *data, size_t len)
{
    _send_i = _send_j = 0;

    for (size_t t = 0; t < len; ++t)
    {
        _send_i %= SHA_DIGEST_LENGTH;
        uint8 x = (data[t] ^ _key[_send_i]) + _send_j;
        _send_i += INC_NUM;
        data[t] = _send_j = x;
    }
}

void AuthCrypt::SetKey(BigNumber *bn)
{
    //uint8 *key = new uint8[SHA_DIGEST_LENGTH];
    //__GenerateKey(key, bn);
    //memcpy(_key, key,sizeof(_key));
    //delete[]key;

    memset(_key,0,sizeof(_key));

    _key[0]  = 'a';
    _key[5]  = 'd';
    _key[6]  = 'c';
    _key[7]  = 'f';
    _key[8]  = 't';
    _key[9]  = 't';

    _key[12] = 'b';

}

void AuthCrypt::__GenerateKey(uint8 *key, BigNumber *bn)
{
    //Sha1Hash hash;
    //hash.UpdateBigNumbers(bn,0);
    //hash.Finalize();
    //memcpy(key, hash.GetDigest(), SHA_DIGEST_LENGTH);

    
}