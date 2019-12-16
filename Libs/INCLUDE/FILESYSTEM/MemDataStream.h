#ifndef _MEMDATASTREAM_H_
#define _MEMDATASTREAM_H_

#include "DataStream.h"

/************************************************************************/
/* 包装内存流对象                                                        */
/************************************************************************/
class CMemDataStream:
    public CDataStream
{
public:
    CMemDataStream(char* pMem,size_t size,bool isDelOnLeave = true);

    ~CMemDataStream(void);

    //method from CDataStream
    virtual size_t Read(void* pDest,size_t num2Read);

    virtual size_t Write(const void* pSrc,size_t num2Write);

    virtual void Skip(long count);

    virtual bool IsEof(void);

    virtual void Close(void);

    virtual size_t Tell(void);

    virtual void Seek(size_t val);

    virtual size_t Size(void) 
    {
        return Tell();
    }

    virtual size_t Capacity(void) 
    {
        return m_size;
    }

    virtual void Write2Stream(DataStreamPtr other);
private:
    char*  m_pData; //指向的数据起始地址
    char*  m_pPos;  //当前指针的位置
    char*  m_pEnd;  //结束位置
    
    bool m_isDelOnLeave; //是否在类销毁时删除内存
};

#endif /*_MEMDATASTREAM_H_*/