#ifndef _ZIPARCHIVE_H_
#define _ZIPARCHIVE_H_

#include "Archive.h"
#include "Factory.h"
#include "DataStream.h"
#include "SimpleZip.h"
#include <hash_map>

/************************************************************************/
/* ZIP�ļ�����                                                          */
/************************************************************************/
class CZipArchive:
    public IArchive
{
    struct ItemInfo
    {
        std::string name; //�ļ�����
        size_t      size; //����
        int         idx;  //��ZIP������
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
    HZIP       m_hZip; //��ǰ����ZIP�ļ�
    ZIPTARGET* m_zipTarget;

    typedef stdext::hash_map<std::string,ItemInfo> ITEMS_MAP;
    ITEMS_MAP m_items;
};

/************************************************************************/
/* ����TPK�ļ�����ʵ��                                                  */
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