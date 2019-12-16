#ifndef _ZIPCOMMON_H_
#define _ZIPCOMMON_H_

#include <windows.h>
#include <string>

#include "DataStream.h"

typedef unsigned char uch;      // unsigned 8-bit value
typedef unsigned short ush;     // unsigned 16-bit value
typedef unsigned long ulg;      // unsigned 32-bit value

#define CHUNK 16384

#define MAX_WBITS   15 /* 32K LZ77 window */

#define DEFAULT_MEM 8

// Lengths of headers after signatures in bytes
#define LOCHEAD 26
#define CENHEAD 42
#define ENDHEAD 18

//一次只能是下面模式种的一种
enum ZIPMODE
{
    E_NONE,
    E_ZIP,
    E_UNZIP
};

/************************************************************************/
/* 统一对文件和对内存的操作                                             */
/************************************************************************/
class CSimpleFileMemWrap
{
public:
    CSimpleFileMemWrap(const char* fileName,bool isTrunck = false);

    CSimpleFileMemWrap(char* pMem,size_t len,bool isDel);

    CSimpleFileMemWrap(DataStreamPtr stream);

    virtual ~CSimpleFileMemWrap(void);

    //返回当前stream大小
    size_t Size(void) {return m_stream->Size();}

    //读取
    size_t Read(void* pDest,size_t num2Read) {return m_stream->Read(pDest,num2Read);}

    template<typename _podType>
    size_t Read(_podType& dest)
    {
        return Read(&dest,sizeof(_podType));
    }

    //写入
    size_t Write(const void* pSrc,size_t num2Write) {return m_stream->Write(pSrc,num2Write);}

    template<typename _podType>
    size_t Write(_podType& src)
    {
        return Write(&src,sizeof(_podType));
    }

    bool IsValid(void) {return !m_stream->IsEof();}

    void Close(void)
    {
        m_stream->Close();
    }

    void SetPos(int pos)
    {
        m_stream->Seek(0);
        m_stream->Seek(pos);
    }

    size_t GetPos(void) {m_stream->Tell();}

    //获取文件信息
    void GetInfo(int& attri,int& timestamp);

    const char* GetPath(void) {return m_path.c_str();}

    virtual void operator = (const CSimpleFileMemWrap& other);

    void SetCmpSize(size_t size) { m_cmpSize = size;} 

    size_t GetCmpSize(void) {return m_cmpSize;}
private:
    void __EnsureDirectory(const char* path);

    bool        m_isFile;    //是否是文件
    int         m_fileAttri; //文件属性
    time_t      m_fileTime;  //文件的修改时间
    std::string m_path;      //保存当前文件的路径
    size_t      m_cmpSize;   //压缩后文件的大小

    DataStreamPtr m_stream;
};

typedef CSimpleFileMemWrap ZIPTARGET;


#endif /*_ZIPCOMMON_H_*/