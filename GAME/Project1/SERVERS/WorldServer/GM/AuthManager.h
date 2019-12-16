#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>
#include <vector>
#include <hash_map>
#include <hash_set>
#include <memory.h>

/************************************************************************/
/* 缓冲GM帐号的权限信息
/************************************************************************/
class CGMAuthManager
{
public:
    typedef std::vector<std::string> AUTHS_MAP;

    // load all auth from DB
    void Load(void);

    // add
    bool AddAuth(int id,const char* role,bool saveDB = true);
    bool DelAuth(int id,const char* role);
    
    bool HasAuth(int id,const char* role);

    AUTHS_MAP GetAuths(int id);
private:
    typedef stdext::hash_map<int,AUTHS_MAP> GM_ROLES_MAP;

    GM_ROLES_MAP m_gmRoles;
};

#endif /*AUTH_MANAGER_H*/