#include "AuthManager.h"
#include "DBLib/dbLib.h"
#include "..\WorldServer.h"

void CGMAuthManager::Load(void)
{
    CDBOperator dbHandle(DATABASE);
    dbHandle->SQL("SELECT * FROM gmRole");

    while(dbHandle->More())
    {
        int id          = dbHandle->GetInt();
        std::string cmd = dbHandle->GetString();

        AddAuth(id,cmd.c_str(),false);
    }
}

bool CGMAuthManager::AddAuth(int id,const char* role,bool saveDB)
{
    if (0 == role)
        return false;

    GM_ROLES_MAP::iterator iter = m_gmRoles.find(id);

    if (iter == m_gmRoles.end())
        iter = m_gmRoles.insert(std::make_pair(id,AUTHS_MAP())).first;

    AUTHS_MAP& roles = iter->second;

    bool isFound = false;

    for(size_t r = 0; r < roles.size(); ++r)
    {
        if (roles[r] == role)
        {
            isFound = true;
            break;
        }
    }

    if (!isFound)
    {
        if (saveDB)
        {
            CDBOperator dbHandle(DATABASE);
            dbHandle->SQL("INSERT INTO gmRole VALUES(%d,'%s')",id,role);

            if (dbHandle->Eval())
            {
                roles.push_back(role);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            roles.push_back(role);
            return true;
        }
    }
    else
    {
        return true;
    }
}

bool CGMAuthManager::DelAuth(int id,const char* role)
{
    if (0 == role)
        return false;

    GM_ROLES_MAP::iterator iter = m_gmRoles.find(id);

    if (iter == m_gmRoles.end())
        return true;

    AUTHS_MAP& roles = iter->second;
    bool isFound     = false;

    for (AUTHS_MAP::iterator iter = roles.begin(); iter != roles.end(); ++iter)
    {
        if (*iter == role)
        {
            roles.erase(iter);
            isFound = true;
            break;
        }
    }

    if (isFound)
    {
        CDBOperator dbHandle(DATABASE);
        dbHandle->SQL("DELETE FROM gmRole WHERE gmFlag=%d AND gmCmd='%s'",id,role);

        if (dbHandle->Eval())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}

bool CGMAuthManager::HasAuth(int id,const char* role)
{
    if (0 == role)
        return false;

    GM_ROLES_MAP::iterator iter = m_gmRoles.find(id);

    if (iter == m_gmRoles.end())
        return false;

    AUTHS_MAP& roles = iter->second;
    bool isFound     = false;

    for (AUTHS_MAP::iterator iter = roles.begin(); iter != roles.end(); ++iter)
    {
        if (*iter == role)
        {
            isFound = true;
            break;
        }
    }

    return isFound;
}

CGMAuthManager::AUTHS_MAP CGMAuthManager::GetAuths(int id)
{
    AUTHS_MAP map;

    GM_ROLES_MAP::iterator iter = m_gmRoles.find(id);

    if (iter == m_gmRoles.end())
        return map;

    return iter->second;
}