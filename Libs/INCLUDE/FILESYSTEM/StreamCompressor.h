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
/* 提供对流压缩的支持                                                   */
/************************************************************************/
class CStreamCompressor
{
public:
    //返回压缩后的内存流
    static DataStreamPtr Compress(DataStreamPtr stream,ECompressMethod method);

    //压缩到指定的流对象
    static bool Compress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method);

    //返回压缩后的内存流
    static DataStreamPtr Compress(DataStreamPtr stream,ECompressMethod method,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite);

    //压缩到指定的流对象
    static bool Compress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite);
private:
    //基于ZLIB的压缩
    static bool __ZlibCompress(DataStreamPtr src,DataStreamPtr dest,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite);
};


#endif /*_STREAMCOMPRESSOR_H_*/