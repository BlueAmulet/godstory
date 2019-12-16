#include "ResGroup.h"
#include "ResMgr.h"

bool CResGroup::LoadAllRes(void)
{
    for (size_t i = 0; i < m_resources.size(); i++)
    {
        if (m_resources[i]->IsLoaded())
            continue;

        if (!m_resources[i]->Load())
            return false;
    }

    return true;
}

bool CResGroup::UnloadAllRes(void)
{
    for (size_t i = 0; i < m_resources.size(); i++)
    {
        if (!m_resources[i]->IsLoaded() || !m_resources[i]->IsReloadable())
            continue;

        m_resources[i]->Unload();
    }

    return true;
}

void CResGroup::AddRes(ResourcePtr res)
{
    m_resources.push_back(res);
}

void CResGroup::DelRes(ResourcePtr res)
{
    std::vector<ResourcePtr>::iterator iter = std::find(m_resources.begin(),m_resources.end(),res);

    if (iter != m_resources.end())
        m_resources.erase(iter);
}

//----------------------------------------------------------------------------------------------------------------

CResGroupMgr::CResGroupMgr(void)
{
    CreateGroup(DEFAULT_RESOURCE_GROUP);
}

CResGroup* CResGroupMgr::CreateGroup(std::string grpName)
{
    CResGroup* pGroup = new CResGroup(grpName);
    return m_groups.insert(std::make_pair(grpName,pGroup)).first->second;
}

bool CResGroupMgr::LoadGroup(std::string grpName)
{
    CResGroup* pGroup = __GetGroup(grpName);

    if (NULL == pGroup)
        return false;

    return pGroup->LoadAllRes();
}

bool CResGroupMgr::UnloadGroup(std::string grpName)
{
    CResGroup* pGroup = __GetGroup(grpName);

    if (NULL == pGroup)
        return false;

    return pGroup->UnloadAllRes();
}

bool CResGroupMgr::UnloadGroupUnused(std::string grpName)
{
    //TODO
    return true;
}

bool CResGroupMgr::RegisterResMgr(CResMgr* pResMgr)
{
    if (NULL == pResMgr)
        return false;

    return m_resMgrs.insert(std::make_pair(pResMgr->GetType(),pResMgr)).second;
}

void CResGroupMgr::UnRegisterResMgr(std::string type)
{
    RESMGRS_MAP::iterator iter = m_resMgrs.find(type);

    if (iter == m_resMgrs.end())
        return;

    m_resMgrs.erase(iter);
}

bool CResGroupMgr::AddRes2Group(std::string grpName,ResourcePtr res)
{
    CResGroup* pGroup = __GetGroup(grpName);

    if (NULL == pGroup)
        pGroup = __GetGroup(DEFAULT_RESOURCE_GROUP);

    pGroup->AddRes(res);
    return true;
         
}

void CResGroupMgr::RemoveFromGroup(ResourcePtr res)
{
    const std::string& grpName = res->GetGroup();

    CResGroup* pGroup = __GetGroup(grpName);

    if (NULL == pGroup)
        return;

    pGroup->DelRes(res);
}

bool CResGroupMgr::AddResDecl(std::string grpName,std::string resName,std::string resType)
{
    CResMgr* pResMgr = __GetResMgr(resType);

    if (NULL == pResMgr)
        return false;

    CResGroup* pGroup = __GetGroup(grpName);

    if (NULL == pGroup)
        pGroup = CreateGroup(grpName);

    ResourcePtr res = pResMgr->DeclareResource(resName,grpName);
    pGroup->AddRes(res);
    return true;
}

CResGroup* CResGroupMgr::__GetGroup(std::string grpName)
{
    GROUPS_MAP::iterator iter = m_groups.find(grpName);

    if (iter == m_groups.end())
        return NULL;

    return iter->second;
}

CResMgr* CResGroupMgr::__GetResMgr(std::string& type)
{
    RESMGRS_MAP::iterator iter = m_resMgrs.find(type);

    if (iter == m_resMgrs.end())
        return NULL;

    return iter->second;
}