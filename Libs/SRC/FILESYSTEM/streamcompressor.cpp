#include "StreamCompressor.h"
#include "MemDataStream.h"

#include "zlib/zlib.h"
#include "ZipCommon.h"

void NullOnReadWrite(char*,size_t) {}

//返回压缩后的内存流
DataStreamPtr CStreamCompressor::Compress(DataStreamPtr stream,ECompressMethod method)
{
    return Compress(stream,method,CMP_CALLBACK(NullOnReadWrite),CMP_CALLBACK(NullOnReadWrite));
}

DataStreamPtr CStreamCompressor::Compress(DataStreamPtr stream,ECompressMethod method,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite)
{
    //创建1.5倍大小的内存流
    size_t allocSize = stream->Size() + stream->Size() * 0.5;

    if (allocSize == 0)
    {
        char* pBuf = new char[1];
        pBuf[0] = 0;
        DataStreamPtr memStream = new CMemDataStream(pBuf,1);
        return memStream;
    }

    char* pBuf = new char[allocSize];

    DataStreamPtr memStream = new CMemDataStream(pBuf,allocSize);

    if (!Compress(stream,memStream,method,onRead,onWrite))
        return NULL;

    return memStream;
}

//压缩到指定的流对象
bool CStreamCompressor::Compress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method)
{
    return Compress(src,dest,method,CMP_CALLBACK(NullOnReadWrite),CMP_CALLBACK(NullOnReadWrite));
}

bool CStreamCompressor::Compress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite)
{
    switch(method)
    {
    case E_COMPRESS_ZLIB:
        {
            return __ZlibCompress(src,dest,onRead,onWrite);
        }
        break;
    }
    return false;   
}

/************************************************************************/
/* 通过zlib的deflate函数进行压缩                                        */
/************************************************************************/
bool CStreamCompressor::__ZlibCompress(DataStreamPtr src,DataStreamPtr dest,CMP_CALLBACK& onRead,CMP_CALLBACK& onWrite)
{
    src->Seek(0);

    static char buf[CHUNK];

    z_stream strm;
    ZeroMemory(&strm,sizeof(strm));

    //strm.avail_out = buf;

    strm.zalloc = Z_NULL;
    strm.opaque = Z_NULL;
    strm.zfree  = Z_NULL;

    //对于在内存上操作deflate的,需要调用deflateInit2
    if (Z_OK != deflateInit2(&strm,Z_BEST_COMPRESSION,Z_DEFLATED,-MAX_WBITS,DEFAULT_MEM,Z_DEFAULT_STRATEGY))
        return false;

    int flushMode     = Z_NO_FLUSH;
    size_t fileSize    = src->Size();
    size_t totalReaded = 0;

    do
    {
        strm.avail_in = src->Read(buf,CHUNK);

        onRead(buf,strm.avail_in);
        totalReaded += strm.avail_in;

        //读入失败
        if (strm.avail_in < 0)
        {
            deflateEnd(&strm);
            return false;
        }

        //是否已经到文件末尾
        if(fileSize == totalReaded)
        {
            flushMode = Z_FINISH;
        }

        strm.next_in = (Bytef*)buf;

        static char tmpOut[CHUNK] = {0};

        do
        {
            strm.next_out  = (Bytef*)tmpOut;
            strm.avail_out = CHUNK;

            deflate(&strm,flushMode);

            unsigned int writtens = CHUNK - strm.avail_out;

            onWrite(tmpOut,writtens);

            unsigned int cout = dest->Write(tmpOut,writtens);

            if (cout != writtens)   //写文件失败
            {
                deflateEnd(&strm);
                return false;
            }
        }
        while(strm.avail_out == 0); //无法输出

    }
    while(flushMode != Z_FINISH);


    deflateEnd(&strm);

    return true;
}