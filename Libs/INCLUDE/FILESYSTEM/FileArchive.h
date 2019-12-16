#ifndef _FILEARCHIVE_H_
#define _FILEARCHIVE_H_

#include "Archive.h"
#include "Factory.h"

/************************************************************************/
/*本地文件系统容器                                                      */
/************************************************************************/
class CFileArchive:
    public IArchive
{
public:
    CFileArchive(std::string path,std::string type);

    ~CFileArchive(void);

    //method from IArchive
    virtual DataStreamPtr Open(std::string fileName);

    virtual bool FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive);
};

/************************************************************************/
/* 创建系统文件容器实例                                                  */
/************************************************************************/

static const std::string FILE_TYPE = "filesystem";

class CFileArchiveFactory:
    public CArchiveFactory
{
public:
    virtual const std::string& GetType(void) const
    {
        return FILE_TYPE;
    }

    virtual IArchive* CreateInstance( const std::string& name )
    {
        return new CFileArchive(name,FILE_TYPE);
    }

    virtual void DestroyInstance(IArchive* pArchive)
    {
        delete pArchive;
    }
};

#endif /*_FILEARCHIVE_H_*/