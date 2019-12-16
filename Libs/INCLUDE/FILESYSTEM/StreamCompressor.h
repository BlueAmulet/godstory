#ifndef _STREAMCOMPRESSOR_H_
#define _STREAMCOMPRESSOR_H_

#include "DataStream.h"
#include "loki/functor.h"

#define BIT(x) ( 1 << x)

enum ECompressMethod
{
    E_COMPRESS_NONE = BIT(0),
    E_COMPRESS_ZLIB = BIT(1)
};

typedef Loki::Functor<void,LOKI_TYPELIST_2(char*,size_t)> CMP_CALLBACK;

/************************************************************************/
/* �ṩ����ѹ����֧��                                                   */
/************************************************************************/
class CStreamCompressor
{
public:
    //����ѹ������ڴ���
    static DataStreamPtr Compress(DataStreamPtr stream,ECompressMethod method);

    //ѹ����ָ����������
    static bool Compress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method);

    //����ѹ������ڴ���
    static DataStreamPtr Compress(DataStreamPtr stream,ECompressMethod method,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite);

    //ѹ����ָ����������
    static bool Compress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite);
private:
    //����ZLIB��ѹ��
    static bool __ZlibCompress(DataStreamPtr src,DataStreamPtr dest,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite);
};


#endif /*_STREAMCOMPRESSOR_H_*/