#include "MemDataStream.h"

CMemDataStream::CMemDataStream(char* pMem,size_t size,bool isDelOnLeave):
    CDataStream(size),
    m_pData(pMem),
    m_pPos(pMem),
    m_pEnd(pMem + size),
    m_isDelOnLeave(isDelOnLeave)
{
}

CMemDataStream::~CMemDataStream(void)
{
    Close();
}

//method from CDataStream
size_t CMemDataStream::Read(void* pDest,size_t num2Read)
{
    size_t cnt = num2Read;

    if (m_pPos + cnt > m_pEnd)
        cnt = m_pEnd - m_pPos;

    if (cnt == 0)
        return 0;

    memcpy(pDest, m_pPos, cnt);
    m_pPos += cnt;
    return cnt;
}

size_t CMemDataStream::Write(const void* pSrc,size_t num2Write)
{
    size_t cnt = num2Write;

    if (m_pPos + cnt > m_pEnd)
        cnt = m_pEnd - m_pPos;

    if (cnt == 0)
        return 0;

    memcpy(m_pPos, pSrc, cnt);
    m_pPos += cnt;
    return cnt;
}

void CMemDataStream::Skip(long count)
{
    long cnt = count;

    if (m_pPos + cnt > m_pEnd)
        cnt = static_cast<long>(m_pEnd - m_pPos);

    if (m_pPos + cnt < m_pData)
        cnt = static_cast<long>(m_pPos - m_pData);

    m_pPos = m_pPos + cnt;
}

bool CMemDataStream::IsEof(void)
{
    return m_pPos >= m_pEnd;
}

void CMemDataStream::Close(void)
{
    if (m_isDelOnLeave && NULL != m_pData)
        delete []m_pData;

    m_pData = NULL;
    m_pPos  = NULL;
    m_pEnd  = NULL;
}

size_t CMemDataStream::Tell(void)
{
    return m_pPos - m_pData;
}

void CMemDataStream::Seek(size_t val)
{
    assert(m_pData + val <= m_pEnd);

    if (m_pData + val > m_pEnd)
        return;

    m_pPos = m_pData + val;
}

void CMemDataStream::Write2Stream(DataStreamPtr other)
{
    other->Write(m_pData,m_pEnd - m_pData);
}