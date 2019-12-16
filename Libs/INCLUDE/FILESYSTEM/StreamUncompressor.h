#ifndef _STREAMUNCOMPRESSOR_H_
#define _STREAMUNCOMPRESSOR_H_

#include "DataStream.h"
#include "StreamCompressor.h"

/************************************************************************/
/* �ṩ������ѹ����֧��
/************************************************************************/
class CStreamUncompressor
{
public:
    //����ѹ������ڴ���
    static DataStreamPtr Uncompress(DataStreamPtr stream,size_t size,ECompressMethod method);

    //ѹ����ָ����������
    static bool Uncompress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method);

private:
    static bool __Uncompress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method,size_t cmpSize);

    //����ZLIB��ѹ��
    static bool __ZlibUncompress(DataStreamPtr src,DataStreamPtr dest,size_t cmpSize);
};

#endif /*_STREAMUNCOMPRESSOR_H_*/