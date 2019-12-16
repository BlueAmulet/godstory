#ifndef _MEMDATASTREAM_H_
#define _MEMDATASTREAM_H_

#include "DataStream.h"

/************************************************************************/
/* ��װ�ڴ�������                                                        */
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
    char*  m_pData; //ָ���������ʼ��ַ
    char*  m_pPos;  //��ǰָ���λ��
    char*  m_pEnd;  //����λ��
    
    bool m_isDelOnLeave; //�Ƿ���������ʱɾ���ڴ�
};

#endif /*_MEMDATASTREAM_H_*/