#ifndef _FILEDATASTREAM_H_
#define _FILEDATASTREAM_H_

#include "DataStream.h"
#include <fstream>

/************************************************************************/
/* 包装文件流对象                                                        */
/************************************************************************/
class CFileDataStream:
    public CDataStream
{
public:
    CFileDataStream(std::fstream* fs,bool isCloseOnLeave = true);

    ~CFileDataStream(void);

    //method from CDataStream
    virtual size_t Read(void* pDest,size_t num2Read);

    virtual size_t Write(const void* pSrc,size_t num2Write);

    virtual void Skip(long count);

    virtual bool IsEof(void);

    virtual void Close(void);

    virtual size_t Tell(void);

    virtual void Seek(size_t val);

    virtual void Flush(void);

    virtual void Write2Stream(DataStreamPtr other);
private:
    std::fstream* m_fs;
    bool m_isCloseOnLeave; //是否在类销毁时关闭文件
};

#endif /*_FILEDATASTREAM_H_*/