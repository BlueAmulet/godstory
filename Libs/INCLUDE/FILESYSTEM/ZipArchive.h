#ifndef _ZIPARCHIVE_H_
#define _ZIPARCHIVE_H_

#include "Archive.h"
#include "Factory.h"
#include "DataStream.h"
#include "SimpleZip.h"
#include <hash_map>

/************************************************************************/
/* ZIP文件容器                                                          */
/************************************************************************/
class CZipArchive:
    public IArchive
{
    struct ItemInfo
    {
        std::string name; //文件名称
        size_t      size; //长度
        int         idx;  //在ZIP里的序号
    };
public:
    CZipArchive(std::string path,std::string type);

    ~CZipArchive(void);

    //method from IArchive
    virtual DataStreamPtr Open(std::string fileName);

    virtual bool FindFileInfo(std::vector<stFileInfo>& infos,std::string pattern,bool isRecursive);

    virtual bool OnLoad(void);

    virtual bool OnLoad(DataStreamPtr stream);

    virtual void OnUnload(void);
private:
    HZIP       m_hZip; //当前包的ZIP文件
    ZIPTARGET* m_zipTarget;

    typedef stdext::hash_map<std::string,ItemInfo> ITEMS_MAP;
    ITEMS_MAP m_items;
};

/************************************************************************/
/* 创建TPK文件容器实例                                                  */
/************************************************************************/

static const std::string ZIP_TYPE = "zipfiles";

class CZipArchiveFactory:
    public CArchiveFactory
{
public:
    virtual const std::string& GetType(void) const
    {
        return ZIP_TYPE;
    }

    virtual IArchive* CreateInstance( const std::string& name )
    {
        return new CZipArchive(name,ZIP_TYPE);
    }

    virtual void DestroyInstance(IArchive* pArchive)
    {
        delete pArchive;
    }
};

#endif /*_ZIPARCHIVE_H_*/