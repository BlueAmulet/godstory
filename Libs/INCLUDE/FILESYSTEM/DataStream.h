#ifndef _DATASTREAM_H_
#define _DATASTREAM_H_

#include <string>
#include "Loki\SmartPtr.h"

class CDataStream;
typedef Loki::SmartPtr<CDataStream> DataStreamPtr;

/************************************************************************/
/* ��װ��ȡ������                                                       */
/************************************************************************/
class CDataStream
{
public:
    CDataStream(void) : m_size(0) {}
    CDataStream(size_t size) : m_size(size) {}

    virtual ~CDataStream(void) {m_size = 0;}

    //���ص�ǰstream��С
    virtual size_t Size(void) {return m_size;}

    //���ص�ǰ��������
    virtual size_t Capacity(void) {return m_size;}

    //��ȡ
    virtual size_t Read(void* pDest,size_t num2Read) = 0;

    template<typename _podType>
    size_t Read(_podType& dest)
    {
        return Read(&dest,sizeof(_podType));
    }

    //д��
    virtual size_t Write(const void* pSrc,size_t num2Write) = 0;

    template<typename _podType>
    size_t Write(_podType& src)
    {
        return Write(&src,sizeof(_podType));
    }

    //��ȡ�ַ���
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

    //��ȡ1���ı�
    virtual std::string ReadLine(void);

    virtual void Skip(long count) = 0;

    virtual bool IsEof(void) = 0;

    virtual void Close(void) = 0;

    virtual size_t Tell(void) = 0;

    virtual void Seek(size_t val) = 0;

    virtual void Flush(void) {}

    //д��������
    virtual void Write2Stream(DataStreamPtr other) = 0;
protected:
    size_t      m_size; //��ǰ������Ĵ�С
};

#endif /*_DATASTREAM_H_*/