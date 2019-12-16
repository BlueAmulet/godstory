#include "FileDataStream.h"

CFileDataStream::CFileDataStream(std::fstream* fs,bool isCloseOnLeave):
    CDataStream(0),
    m_fs(fs),
    m_isCloseOnLeave(isCloseOnLeave)
{
    //也许前面这个流经过多次打开。存有失败标记，先清楚
    m_fs->clear();

    m_fs->seekg(0, std::ios_base::end);
    m_size = m_fs->tellg();
    m_fs->seekg(0, std::ios_base::beg);
}

CFileDataStream::~CFileDataStream(void)
{
    Close();
}

//method from CDataStream
size_t CFileDataStream::Read(void* pDest,size_t num2Read)
{
    if (IsEof())
        return 0;

    m_fs->read(static_cast<char*>(pDest), static_cast<std::streamsize>(num2Read));
    return m_fs->gcount();
}

size_t CFileDataStream::Write(const void* pSrc,size_t num2Write)
{
    m_fs->write(static_cast<const char*>(pSrc),static_cast<std::streamsize>(num2Write));

    if (m_fs->good())
    {
        m_size += num2Write;
        return num2Write;
    }
    else
        return 0;
}

void CFileDataStream::Skip(long count)
{
    m_fs->clear(); //Clear fail status in case eof was set
    m_fs->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
}

bool CFileDataStream::IsEof(void)
{
    return m_fs->eof();
}

void CFileDataStream::Close(void)
{
    if (m_fs && m_isCloseOnLeave && m_fs->is_open())
    {
        m_fs->close();
        delete m_fs;
        m_fs = NULL;
    }
}

size_t CFileDataStream::Tell(void)
{
    return m_fs->tellg();
}

void CFileDataStream::Seek(size_t val)
{
    m_fs->clear();
    m_fs->seekg(static_cast<std::streamoff>(val), std::ios::beg);
}

void CFileDataStream::Flush(void)
{
    m_fs->flush();
}

void CFileDataStream::Write2Stream(DataStreamPtr other)
{
    Seek(0);
    char* pBuf = new char[Size()];
    Read(pBuf,Size());
    other->Write(pBuf,Size());
    delete []pBuf;
}