#include "StreamUncompressor.h"
#include "MemDataStream.h"
#include <assert.h>

#include "zlib/zlib.h"
#include "ZipCommon.h"

//返回解压缩后的内存流
DataStreamPtr CStreamUncompressor::Uncompress(DataStreamPtr stream,size_t size,ECompressMethod method)
{
    if (0 == size)
        return NULL;

    char* pBuf = new char[size];
    DataStreamPtr memStream = new CMemDataStream(pBuf,size);

    return __Uncompress(stream,memStream,method,stream->Capacity()) ? memStream : NULL;
}

//解压缩到指定的流对象
bool CStreamUncompressor::Uncompress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method)
{
    assert(dest->Capacity() >= src->Size());

    return __Uncompress(src,dest,method,src->Size());
}

bool CStreamUncompressor::__Uncompress(DataStreamPtr src,DataStreamPtr dest,ECompressMethod method,size_t cmpSize)
{
    switch(method)
    {
    case E_COMPRESS_ZLIB:
        {
            return __ZlibUncompress(src,dest,cmpSize);
        }
        break;
    }
    return false;   
}

/************************************************************************/
/* 通过zlib的deflate函数进行压缩                                        */
/************************************************************************/
bool CStreamUncompressor::__ZlibUncompress(DataStreamPtr src,DataStreamPtr dest,size_t cmpSize)
{
    src->Seek(0);
    z_stream      strm;

    ZeroMemory(&strm,sizeof(strm));

    if (Z_OK != inflateInit2(&strm,-MAX_WBITS))
        return false;

    size_t byte2Read   = CHUNK;
    size_t byteReaded  = 0;

    static char buf[CHUNK] = {0};

    while(byteReaded != cmpSize)
    {
        if (byte2Read + byteReaded > cmpSize)
        {
            byte2Read = cmpSize - byteReaded;
        }

        size_t dwReaded = src->Read(buf,byte2Read);
        byteReaded += dwReaded;

        strm.next_in  = (Bytef*)buf;
        strm.avail_in = dwReaded;

        static char tmpOut[CHUNK] = {0};
        unsigned have = 0;  //读入的数据量

        do
        {
            strm.next_out  = (Bytef*)tmpOut;
            strm.avail_out = CHUNK;

            int ret = inflate(&strm, Z_SYNC_FLUSH);

            switch (ret) 
            {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                {
                    inflateEnd(&strm);
                    return false;
                }
            }

            have = CHUNK - strm.avail_out;

            if (have != dest->Write(tmpOut,have))
            {
                inflateEnd(&strm);
                return false;
            }
        }
        while(strm.avail_out == 0);
    }

    inflateEnd(&strm);

    return true;
}