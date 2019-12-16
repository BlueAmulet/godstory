#ifndef _UPDATEPKT_H_
#define _UPDATEPKT_H_

#include <string>
#include "DataStream.h"
#include <hash_map>
#include "TpkIndexFile.h"

#define UPDATEPKTFLAG 0x00100001
#define BIT(x) ( 1 << x)

struct FileItem
{
    std::string   name;
    size_t        size;
    DataStreamPtr data;
};

 
/************************************************************************/
/* 更新包结构定义
/************************************************************************/
struct CUpdatePkt
{
public:
    CUpdatePkt(void);

    ~CUpdatePkt(void);

    bool Read(DataStreamPtr& stream);

    bool Write(DataStreamPtr& stream);

    //增加文件包
    bool AddFile(FileItem& file);

    //获取更新后版本
    std::string GetVersionTo(void) {return m_versionTo;}

    //获取起始版本号
    std::string GetVersionFrom(void) {return m_versionFrom;}

    //获取索引表
    std::vector<std::string>& GetIdxFiles(void);

    //获取文件数据
    DataStreamPtr GetData(const char* fileName,int& flag,size_t& size);
public:
    bool __InitIdxMap(void);

    int  m_flag;                  //标识更新包
    std::string m_versionFrom;    //更新起始版本，由主版本号和次版本号组合
    std::string m_versionTo;      //更新后客户端版本号
    std::string m_describe;       //更新包描述
    int  m_type;                  //包类型 BIT(0):普通更新文件 BIT(1): EXE程序

    typedef stdext::hash_map<std::string,FileItem> FILES_MAP;
    FILES_MAP m_fileItems;

    std::vector<std::string> m_idxFiles;
    CTpkIndexFile* m_pIdxFile;
};

#endif /*_UPDATEPKT_H_*/