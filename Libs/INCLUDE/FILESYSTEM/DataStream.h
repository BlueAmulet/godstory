#ifndef _DATASTREAM_H_
#define _DATASTREAM_H_

#include <string>
#include "Loki\SmartPtr.h"

class CDataStream;
typedef Loki::SmartPtr<CDataStream> DataStreamPtr;

/************************************************************************/
/* 包装读取流对象                                                       */
/************************************************************************/
class CDataStream
{
public:
    CDataStream(void) : m_size(0) {}
    CDataStream(size_t size) : m_size(size) {}

    virtual ~CDataStream(void) {m_size = 0;}

    //返回当前stream大小
    virtual size_t Size(void) {return m_size;}

    //返回当前流的容量
    virtual size_t Capacity(void) {return m_size;}

    //读取
    virtual size_t Read(void* pDest,size_t num2Read) = 0;

    template<typename _podType>
    size_t Read(_podType& dest)
    {
        return Read(&dest,sizeof(_podType));
    }

    //写入
    virtual size_t Write(const void* pSrc,size_t num2Write) = 0;

    template<typename _podType>
    size_t Write(_podType& src)
    {
        return Write(&src,sizeof(_podType));
    }

    //读取字符串
    bool ReadString(char* buf,size_t bufLen)
    {
        size_t len = 0;

        if (0 == Read(len))
            return false;

        if (len >= bufLen)
            return false;
        
        buf[len] = 0;
        return Read(buf,len) == len;
    }

    bool ReadString(std::string& rslt)
    {
        size_t len = 0;

        if (0 == Read(len))
            return false;

        char* pBuf = new char[len + 1];

        if (len != Read(pBuf,len))
        {
            delete []pBuf;
            return false;
        }

        pBuf[len] = 0;
        rslt = pBuf;
        delete []pBuf;
        return true;
    }

    bool WriteString(std::string& content)
    {
        size_t strLen  = content.length();
        Write(strLen);

        return strLen == Write(content.c_str(),strLen);
    }

    //读取1行文本
    virtual std::string ReadLine(void);

    virtual void Skip(long count) = 0;

    virtual bool IsEof(void) = 0;

    virtual void Close(void) = 0;

    virtual size_t Tell(void) = 0;

    virtual void Seek(size_t val) = 0;

    virtual void Flush(void) {}

    //写到流数据
    virtual void Write2Stream(DataStreamPtr other) = 0;
protected:
    size_t      m_size; //当前流对象的大小
};

#endif /*_DATASTREAM_H_*/