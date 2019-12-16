#ifndef _ARCHIVEMGR_H_
#define _ARCHIVEMGR_H_

#include "Loki\\singleton.h"
#include "DataStream.h"

#include <hash_map>

class CArchiveFactory;
class IArchive;

/************************************************************************/
/* ����ǰ���е��ļ�����,�Ͳ�����                                       */
/************************************************************************/
class CArchiveMgr
{
public:
    CArchiveMgr(void);

    ~CArchiveMgr(void);

    //��������
    IArchive* Load(std::string path,std::string type);

    //�����м�������
    IArchive* Load(DataStreamPtr stream,std::string path,std::string type);

    IArchive* GetArchive(std::string path);

    void Unload(IArchive* pArchive);

    void Unload(std::string path);

    void UnloadAll(void); //�ڽ���ʱ�������

    //��ӹ���
    void AddFactory(CArchiveFactory* pFactory); //ͨ����������չarchive����
private:
    typedef stdext::hash_map<std::string,IArchive*>        ARCHIVES_MAP;        
    typedef stdext::hash_map<std::string,CArchiveFactory*> ARCHIVEFACTORYS_MAP;

    ARCHIVES_MAP::iterator __Unload(std::string path);

    ARCHIVES_MAP        m_archives;             //�����б�
    ARCHIVEFACTORYS_MAP m_archivesFactorys;     //�����б�
};

typedef Loki::SingletonHolder<CArchiveMgr> ArchiveMgr;

#endif /*_ARCHIVEMGR_H_*/