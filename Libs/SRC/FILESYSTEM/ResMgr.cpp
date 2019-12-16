#include "ResMgr.h"
#include "ResGroup.h"

unsigned int CResMgr::m_resHandle = 1;

CResMgr::CResMgr(std::string type):
    m_type(type)
{
}

CResMgr::~CResMgr(void)
{
    m_resources.clear();
}

ResourcePtr CResMgr::DeclareResource(std::string resName,std::string grp)
{
    ResourcePtr rslt = GetRes(resName);

    if (rslt)
        return rslt;

    rslt = __CreateResImpl(resName,grp,__AllocHandle());
    m_resources.insert(std::make_pair(resName,rslt));

    return rslt;
}

ResourcePtr CResMgr::LoadResource(std::string resName,std::string grp)
{
    ResourcePtr rslt = GetRes(resName);

    if (rslt)
    {
        if (!rslt->IsLoaded())
            rslt->Load();

        return rslt;
    }

    rslt = __CreateResImpl(resName,grp,__AllocHandle());
    m_resources.insert(std::make_pair(resName,rslt));
    rslt->Load();

    //把资源加入到组中
    ResGrpMgr::Instance().AddRes2Group(grp,rslt);
    return rslt;
}

void CResMgr::UnloadResource(std::string resName)
{
    RES_MAP::iterator iter = m_resources.find(resName);

    if (iter == m_resources.end())
        return;

    ResourcePtr res = iter->second;
    res->Unload(); //卸载资源

    m_resources.erase(iter);

    //从组里面删除
    ResGrpMgr::Instance().RemoveFromGroup(res);
}

ResourcePtr CResMgr::GetRes(const std::string& resName)
{
    RES_MAP::iterator iter = m_resources.find(resName);

    if (iter == m_resources.end())
        return NULL;

    return iter->second;
}

RESHANDLE CResMgr::__AllocHandle(void)
{
    return m_resHandle++;
}

ResourcePtr CResMgr::__CreateResImpl(std::string resName,std::string grp,RESHANDLE handle)
{
    return new CResource(this,resName,m_type,grp,handle);
}