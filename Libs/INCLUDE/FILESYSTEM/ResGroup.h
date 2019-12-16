#ifndef _RESGROUP_H_
#define _RESGROUP_H_

#include <string>
#include <hash_map>
#include "Resource.h"

#include "Loki\\singleton.h"

#define DEFAULT_RESOURCE_GROUP "Default"

class CResMgr;

/************************************************************************/
/* 资源组                                                                     */
/************************************************************************/
class CResGroup
{
public:
    CResGroup(std::string name):
      m_name(name) {}

    bool LoadAllRes(void);

    bool UnloadAllRes(void);

    void AddRes(ResourcePtr res);

    void DelRes(ResourcePtr res);
private:
    std::string m_name; //组名
    std::vector<ResourcePtr> m_resources;
};

/************************************************************************/
/* 对资源进行分组管理                                                   */
/************************************************************************/
class CResGroupMgr
{
public:
    CResGroupMgr(void);
    //创建组
    CResGroup* CreateGroup(std::string grpName);

    //加载组内所有的资源
    bool LoadGroup(std::string grpName);

    //卸载组内所有资源
    bool UnloadGroup(std::string grpName);

    //卸载组内所有没有用到的资源
    bool UnloadGroupUnused(std::string grpName);

    //把资源加到组里,找不到加入默认组内
    bool AddRes2Group(std::string grpName,ResourcePtr res);

    //从组中删除
    void RemoveFromGroup(ResourcePtr res);

    //增加资源定义
    bool AddResDecl(std::string grpName,std::string resName,std::string resType);

    //注册资源管理器
    bool RegisterResMgr(CResMgr* pResMgr);

    void UnRegisterResMgr(std::string type);
private:
    CResGroup* __GetGroup(std::string grpName);

    CResMgr*   __GetResMgr(std::string& type);

    typedef stdext::hash_map<std::string,CResGroup*> GROUPS_MAP;
    GROUPS_MAP  m_groups;

    typedef stdext::hash_map<std::string,CResMgr*>   RESMGRS_MAP;
    RESMGRS_MAP m_resMgrs;
};

typedef Loki::SingletonHolder<CResGroupMgr> ResGrpMgr;

#endif /*_RESGROUP_H_*/