#ifndef _ARCHIVEMGR_H_
#define _ARCHIVEMGR_H_

#include "Loki\\singleton.h"
#include "DataStream.h"

#include <hash_map>

class CArchiveFactory;
class IArchive;

/************************************************************************/
/* 管理当前所有的文件容器,和产生器                                       */
/************************************************************************/
class CArchiveMgr
{
public:
    CArchiveMgr(void);

    ~CArchiveMgr(void);

    //载入容器
    IArchive* Load(std::string path,std::string type);

    //从流中加载容器
    IArchive* Load(DataStreamPtr stream,std::string path,std::string type);

    IArchive* GetArchive(std::string path);

    void Unload(IArchive* pArchive);

    void Unload(std::string path);

    void UnloadAll(void); //在结束时必须调用

    //添加工厂
    void AddFactory(CArchiveFactory* pFactory); //通过工厂来扩展archive类型
private:
    typedef stdext::hash_map<std::string,IArchive*>        ARCHIVES_MAP;        
    typedef stdext::hash_map<std::string,CArchiveFactory*> ARCHIVEFACTORYS_MAP;

    ARCHIVES_MAP::iterator __Unload(std::string path);

    ARCHIVES_MAP        m_archives;             //容器列表
    ARCHIVEFACTORYS_MAP m_archivesFactorys;     //工厂列表
};

typedef Loki::SingletonHolder<CArchiveMgr> ArchiveMgr;

#endif /*_ARCHIVEMGR_H_*/