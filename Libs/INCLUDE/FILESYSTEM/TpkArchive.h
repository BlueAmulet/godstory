#ifndef _TPKARCHIVE_H_
#define _TPKARCHIVE_H_

#include "Archive.h"
#include "Factory.h"
#include "DataStream.h"
#include "TpkStruct.h"
#include <hash_map>

/************************************************************************/
/* TPK文件容器                                                          */
/************************************************************************/
class CTpkArchive:
    public IArchive
{
public:
    CTpkArchive(std::string path,std::string type);

    ~CTpkArchive(void);

    //method from IArchive
    virtual DataStreamPtr Open(std::string fileName);

    virtual bool FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive);

    virtual bool OnLoad(void);

    virtual bool OnLoad(DataStreamPtr stream); //load from stream

    virtual void OnUnload(void);
private:
    DataStreamPtr m_fstream; //TPK文件流

    //读入的tpk信息
    CTpkHeader            m_header;

    typedef stdext::hash_map<std::string,CTpkItem> ITEMS_MAP;
    ITEMS_MAP m_items;
};

/************************************************************************/
/* 创建TPK文件容器实例                                                  */
/************************************************************************/

static const std::string TPK_TYPE = "tpkfiles";

class CTpkArchiveFactory:
    public CArchiveFactory
{
public:
    virtual const std::string& GetType(void) const
    {
        return TPK_TYPE;
    }

    virtual IArchive* CreateInstance( const std::string& name )
    {
        return new CTpkArchive(name,TPK_TYPE);
    }

    virtual void DestroyInstance(IArchive* pArchive)
    {
        delete pArchive;
    }
};

#endif /*_TPKARCHIVE_H_*/