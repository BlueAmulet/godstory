#ifndef _RESGROUP_H_
#define _RESGROUP_H_

#include <string>
#include <hash_map>
#include "Resource.h"

#include "Loki\\singleton.h"

#define DEFAULT_RESOURCE_GROUP "Default"

class CResMgr;

/************************************************************************/
/* ��Դ��                                                                     */
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
    std::string m_name; //����
    std::vector<ResourcePtr> m_resources;
};

/************************************************************************/
/* ����Դ���з������                                                   */
/************************************************************************/
class CResGroupMgr
{
public:
    CResGroupMgr(void);
    //������
    CResGroup* CreateGroup(std::string grpName);

    //�����������е���Դ
    bool LoadGroup(std::string grpName);

    //ж������������Դ
    bool UnloadGroup(std::string grpName);

    //ж����������û���õ�����Դ
    bool UnloadGroupUnused(std::string grpName);

    //����Դ�ӵ�����,�Ҳ�������Ĭ������
    bool AddRes2Group(std::string grpName,ResourcePtr res);

    //������ɾ��
    void RemoveFromGroup(ResourcePtr res);

    //������Դ����
    bool AddResDecl(std::string grpName,std::string resName,std::string resType);

    //ע����Դ������
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