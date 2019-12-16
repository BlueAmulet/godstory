#include "Resource.h"
#include "FilesMap.h"
#include "Archive.h"

CResource::CResource(void):
    m_name(""),
    m_pResMgr(NULL),
    m_type(""),
    m_handle(INVALID_RESOUCE_HANDLE),
    m_isLoaded(false),
    m_isReloadable(true)
{
}

CResource::CResource(CResMgr* pResMgr,std::string resName,std::string resType,std::string grp,RESHANDLE handle):
    m_name(resName),
    m_pResMgr(pResMgr),
    m_type(resType),
    m_group(grp),
    m_handle(handle),
    m_isLoaded(false),
    m_isReloadable(true)
{
}

bool CResource::Load(void)
{
    if (IsLoaded())
        return true;

    IArchive* pArchive = ResFileMap::Instance().Find(m_name);

    if (NULL == pArchive)
        return false;

    m_pRes = pArchive->Open(m_name);
    return m_pRes ? true : false;
}

bool CResource::Unload(void)
{
    if(m_pRes)
        m_pRes->Close();

    return true;
}

DataStreamPtr CResource::GetData(void)
{
    if (!IsLoaded())
        Load();

    return m_pRes;
}