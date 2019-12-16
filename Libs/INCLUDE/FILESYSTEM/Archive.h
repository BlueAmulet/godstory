#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "DataStream.h"
#include "Factory.h"

class IArchive;

struct stFileInfo
{
    stFileInfo(void): pArchive(NULL), crc(0),size(0) {}

    IArchive*   pArchive;  //保存方便后面查找
    std::string fullName;  //全路径
    std::string path;      //所在目录
    std::string fileName;  //文件名
    unsigned long crc;     //文件的CRC值
    size_t      size;      //文件的长度
};

/************************************************************************/
/* 文件容器,用来表示1个文件夹或1个ZIP文件,或者是自定义压缩文件          */
/************************************************************************/
class IArchive
{
public:
    IArchive(std::string path,std::string type):
      m_path(path),m_type(type) {}

    virtual ~IArchive(void) {}

    const std::string& GetPath(void) const {return m_path;}

    const std::string& GetType(void) const {return m_type;}

    //virtual methods
    //载入对应的文档,比如打开zip文件
    virtual bool OnLoad(void) {return true;}

    virtual bool OnLoad(DataStreamPtr stream) {return true;}

    //关闭文档对象,eg:关闭zip文件
    virtual void OnUnload(void) {}

    //打开文件:
    virtual DataStreamPtr Open(std::string fileName) = 0;

    //查找文件信息
    virtual bool FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive) = 0;
protected:
    std::string m_path; //当前archive管理的文件路径 eg ..data
    std::string m_type; //文档类型
};

/************************************************************************/
/* archive工厂类,archive在archive manager内部被创建                           */
/************************************************************************/
class CArchiveFactory:
    public IFactory<IArchive>
{
    friend IArchive;
};

#endif /*_ARCHIVE_H_*/