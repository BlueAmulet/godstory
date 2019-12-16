#ifndef _FILEARCHIVE_H_
#define _FILEARCHIVE_H_

#include "Archive.h"
#include "Factory.h"

/************************************************************************/
/*�����ļ�ϵͳ����                                                      */
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
/* ����ϵͳ�ļ�����ʵ��                                                  */
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