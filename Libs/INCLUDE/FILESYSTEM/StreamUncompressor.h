#ifndef _STREAMUNCOMPRESSOR_H_
#define _STREAMUNCOMPRESSOR_H_

#include "DataStream.h"
#include "StreamCompressor.h"

/************************************************************************/
/* 提供对流解压缩的支持
/************************************************************************/
class CStreamUncompressor
{
public:
    //返回压缩后的内存流
    static DataStreamPtr Uncompress(DataStreamPtr stream,size_t size,ECompressMethod method);

    //压缩到指定的流对象
    static bool Uncompress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method);

private:
    static bool __Uncompress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method,size_t cmpSize);

    //基于ZLIB的压缩
    static bool __ZlibUncompress(DataStreamPtr src,DataStreamPtr dest,size_t cmpSize);
};

#endif /*_STREAMUNCOMPRESSOR_H_*/