#include "ArchiveMgr.h"
#include "Archive.h"

CArchiveMgr::CArchiveMgr(void)
{
}

CArchiveMgr::~CArchiveMgr(void)
{
    UnloadAll();
}

//载入容器
IArchive* CArchiveMgr::Load(std::string path,std::string type)
{
    ARCHIVES_MAP::iterator iterArchive = m_archives.find(path);

    if (iterArchive != m_archives.end())
        return iterArchive->second;

    ARCHIVEFACTORYS_MAP::iterator iterFactory = m_archivesFactorys.find(type);

    if (iterFactory == m_archivesFactorys.end())
    {
        assert(0 && "没有找到创建archive的工厂");
        return NULL;
    }

    IArchive* pArchive = iterFactory->second->CreateInstance(path);

    if (NULL != pArchive)
    {
        if (!pArchive->OnLoad())
        {
            //加载失败
            iterFactory->second->DestroyInstance(pArchive);
            return NULL;
        }
        else
            m_archives.insert(std::make_pair(path,pArchive));
    }

    return pArchive;
}

IArchive* CArchiveMgr::Load(DataStreamPtr stream,std::string path,std::string type)
{
    ARCHIVES_MAP::iterator iterArchive = m_archives.find(path);

    if (iterArchive != m_archives.end())
        return iterArchive->second;

    ARCHIVEFACTORYS_MAP::iterator iterFactory = m_archivesFactorys.find(type);

    if (iterFactory == m_archivesFactorys.end())
    {
        assert(0 && "没有找到创建archive的工厂");
        return NULL;
    }

    IArchive* pArchive = iterFactory->second->CreateInstance(path);

    if (NULL != pArchive)
    {
        if (!pArchive->OnLoad(stream))
        {
            //加载失败
            iterFactory->second->DestroyInstance(pArchive);
            return NULL;
        }
        else
            m_archives.insert(std::make_pair(path,pArchive));
    }

    return pArchive;
}

IArchive* CArchiveMgr::GetArchive(std::string path)
{
    ARCHIVES_MAP::iterator iterArchive = m_archives.find(path);

    if (iterArchive != m_archives.end())
        return iterArchive->second;

    return NULL;
}

void CArchiveMgr::Unload(IArchive* pArchive)
{
    if (NULL == pArchive)   
        return;

    Unload(pArchive->GetPath());
}

void CArchiveMgr::Unload(std::string path)
{
    m_archives.erase(__Unload(path));
}

void CArchiveMgr::UnloadAll(void)
{
    for (ARCHIVES_MAP::iterator iterArchive = m_archives.begin(); iterArchive != m_archives.end(); iterArchive++)
    {
        __Unload(iterArchive->first);
    }

    m_archives.clear();
}

void CArchiveMgr::AddFactory(CArchiveFactory* pFactory)
{
    if (NULL == pFactory)
        return;
    
    ARCHIVEFACTORYS_MAP::iterator iterFactory = m_archivesFactorys.find(pFactory->GetType());

    if (iterFactory != m_archivesFactorys.end())
        return;

    m_archivesFactorys.insert(std::make_pair(pFactory->GetType(),pFactory));
}

CArchiveMgr::ARCHIVES_MAP::iterator CArchiveMgr::__Unload(std::string path)
{
    ARCHIVES_MAP::iterator iterArchive = m_archives.find(path);

    if (iterArchive == m_archives.end())
        return iterArchive;

    IArchive* pArchive = iterArchive->second;

    pArchive->OnUnload();

    ARCHIVEFACTORYS_MAP::iterator iterFactory = m_archivesFactorys.find(pArchive->GetType());

    if (iterFactory != m_archivesFactorys.end())
    {
        iterFactory->second->DestroyInstance(pArchive);
    }

    return iterArchive;
}