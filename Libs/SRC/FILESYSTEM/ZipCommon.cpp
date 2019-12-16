#include "ZipCommon.h"
#include <assert.h>
#include "FileDataStream.h"
#include "MemDataStream.h"

#include <sys/types.h>
#include <sys/stat.h>


CSimpleFileMemWrap::CSimpleFileMemWrap(const char* fileName,bool isTrunck):
    m_isFile(true),
    m_path(fileName),
    m_cmpSize(0),
    m_fileAttri(0)
{
    //��֤�ļ�Ŀ¼����
    __EnsureDirectory(fileName);

    std::fstream* fs = new std::fstream;

    if (isTrunck)
        fs->open(fileName,std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    else
    {
        fs->open(fileName,std::ios_base::in | std::ios_base::binary);

        if (!fs->is_open())
            fs->open(fileName,std::ios_base::out | std::ios_base::binary);
    }

    if (!fs->is_open())
    {
        assert(0);
    }

    //��ȡ���ļ���Ӧ����Ϣ
    struct stat tagStat;
    bool ret = (stat(fileName, &tagStat) == 0);

    if (ret)
    {
        if (tagStat.st_mode & _S_IFDIR) 
            m_fileAttri |= 0x10;

        if (tagStat.st_mode & _S_IFDIR) 
            m_fileAttri |= 0x40000000;
        else
            m_fileAttri |= 0x80000000;

        m_fileAttri |= 0x01000000;

        if (tagStat.st_mode & _S_IWRITE) 
            m_fileAttri |= 0x00800000;
        else
            m_fileAttri |= 0x01;
    
        if (tagStat.st_mode & _S_IEXEC) 
            m_fileAttri |= 0x00400000;

        m_fileTime = tagStat.st_mtime;
    }

    m_stream = new CFileDataStream(fs);

    m_stream->Seek(0);
}

CSimpleFileMemWrap::CSimpleFileMemWrap(char* pMem,size_t len,bool isDel):
    m_isFile(false),
    m_cmpSize(0),
    m_fileAttri(0)
{
    m_stream = new CMemDataStream(pMem,len,isDel);
}

CSimpleFileMemWrap::CSimpleFileMemWrap(DataStreamPtr stream):
    m_isFile(false),
    m_cmpSize(0),
    m_fileAttri(0)
{
    m_stream = stream;  
}

CSimpleFileMemWrap::~CSimpleFileMemWrap(void)
{
}

namespace
{
    typedef unsigned long lutime_t;       // define it ourselves since we don't include time.h

    void filetime2dosdatetime(const FILETIME ft, WORD *dosdate,WORD *dostime)
    { 
        // date: bits 0-4 are day of month 1-31. Bits 5-8 are month 1..12. Bits 9-15 are year-1980
        // time: bits 0-4 are seconds/2, bits 5-10 are minute 0..59. Bits 11-15 are hour 0..23
        SYSTEMTIME st; 
        FileTimeToSystemTime(&ft,&st);

        *dosdate = (WORD)(((st.wYear-1980)&0x7f) << 9);
        *dosdate |= (WORD)((st.wMonth&0xf) << 5);
        *dosdate |= (WORD)((st.wDay&0x1f));
        *dostime = (WORD)((st.wHour&0x1f) << 11);
        *dostime |= (WORD)((st.wMinute&0x3f) << 5);
        *dostime |= (WORD)((st.wSecond*2)&0x1f);
    }

    lutime_t filetime2timet(const FILETIME ft)
    { 
        LONGLONG i = *(LONGLONG*)&ft; 
        return (lutime_t)((i-116444736000000000LL)/10000000LL);
    }

    void GetNow(WORD *dosdate, WORD *dostime)
    { 
        SYSTEMTIME st; 
        GetLocalTime(&st);

        FILETIME ft;  
        SystemTimeToFileTime(&st,&ft);
        filetime2dosdatetime(ft,dosdate,dostime);
    }
}

/************************************************************************/
/* �ڱ���ӵ�ZIPʱȡ����Ӧ���ļ���Ϣ                                    */
/************************************************************************/
void CSimpleFileMemWrap::GetInfo(int& attri,int& timestamp)
{
    if (!m_isFile)
    {
        attri = 0x80000000;      // just a normal file

        WORD dosdate, dostime; 
        GetNow(&dosdate, &dostime);
        
        timestamp = (WORD)dostime | (((DWORD)dosdate)<<16);
    }
    else
    {
        attri = m_fileAttri;

        WORD dosdate,dostime;
        FILETIME time;

        time.dwLowDateTime  = m_fileTime & 0xffffffff;
        time.dwHighDateTime = m_fileTime & 0xffffffff00000000;
        filetime2dosdatetime(time,&dosdate,&dostime);
        timestamp = (WORD)dostime | (((DWORD)dosdate)<<16);
    }
}

/************************************************************************/
/* ����俽��,��ʱֻ�ṩ�ļ��俽��                                      */
/************************************************************************/
void CSimpleFileMemWrap::operator = (const CSimpleFileMemWrap& other)
{
    if(m_isFile && other.m_isFile)
    {
        //�ļ��俽��
        ::CopyFile(other.m_path.c_str(),m_path.c_str(),false);
    }
}

/************************************************************************/
/* ��֤�ļ�·���Ĵ���                                                   */
/************************************************************************/
void CSimpleFileMemWrap::__EnsureDirectory(const char* path)
{
    static char dir[_MAX_PATH] = {0};
    static char div[_MAX_PATH] = {0};
    _splitpath(path,dir,div,NULL,NULL);
    strcat(dir,div);

    if (0xFFFFFFFF != GetFileAttributes(path))
        return;

    for (int i = 0; i < strlen(dir); i++)
    {
        if (dir[i] == '\\' || dir[i] == '/')
        {
            char tmp = dir[i];

            dir[i] = 0;
            
            if (0xFFFFFFFF == GetFileAttributes(dir))
            {
                ::CreateDirectory(dir,0);
            }
            
            dir[i] = tmp;
        }
    }
}