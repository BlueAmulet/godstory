#include "BanList.h"
#include "DBLib/dbLib.h"
#include "WorldServer.h"
#include <time.h>

bool CBanlist::Init(void)
{
    CDBOperator dbHandle(DATABASE);
    
    int curTime = int(time(0));
    //首先删除所有过时的ban信息
    dbHandle->SQL("DELETE FROM banInfo WHERE loginBan < %d AND chatBan < %d",curTime,curTime);
    dbHandle->Eval();
    
    //读取所有的ban信息
    dbHandle->SQL("SELECT * FROM banInfo");

    BanInfo banInfo;

    while(dbHandle->More())
    {
        banInfo.name           = __TranslateName(dbHandle->GetString());
        int type               = dbHandle->GetInt();
        banInfo.loginBanExpire = dbHandle->GetInt();
        banInfo.chatBanExpire  = dbHandle->GetInt();
        banInfo.loginReason    = dbHandle->GetString();
        banInfo.chatReason     = dbHandle->GetString();

        if (type >= TARGET_TYPE_COUNT)
            continue;
        
        switch(type)
        {
        case TARGET_TYPE_ACC:
            {   
                m_accBans.insert(std::make_pair(banInfo.name,banInfo));
            }
            break;
        case TARGET_TYPE_ACTOR:
            {
                m_actorBans.insert(std::make_pair(banInfo.name,banInfo));
            }
            break;
        case TARGET_TYPE_IP:
            {
                m_ipBans.insert(std::make_pair(banInfo.name,banInfo));
            }
            break;
        }
    }

    return true;
}

void CBanlist::BanIpLogin(const char* name,int lastTime,const char* reason)
{
    if (0 == name)
        return;

    __DoBan(m_ipBans,TARGET_TYPE_IP,name,BAN_TYPE_LOGIN,lastTime,reason);
}

void CBanlist::BanIpChat(const char* name,int lastTime,const char* reason)
{
    if (0 == name)
        return;

    __DoBan(m_ipBans,TARGET_TYPE_IP,name,BAN_TYPE_CHAT,lastTime,reason);
}

void CBanlist::BanAccLogin(const char* name,int lastTime,const char* reason)
{
    if (0 == name)
        return;

    __DoBan(m_accBans,TARGET_TYPE_ACC,name,BAN_TYPE_LOGIN,lastTime,reason);
}

void CBanlist::BanAccChat(const char* name,int lastTime,const char* reason)
{
    if (0 == name)
        return;

    __DoBan(m_accBans,TARGET_TYPE_ACC,name,BAN_TYPE_CHAT,lastTime,reason);
}

void CBanlist::BanActorLogin(const char* name,int lastTime,const char* reason)
{
    if (0 == name)
        return;

    __DoBan(m_actorBans,TARGET_TYPE_ACTOR,name,BAN_TYPE_LOGIN,lastTime,reason);
}

void CBanlist::BanActorChat(const char* name,int lastTime,const char* reason)
{
    if (0 == name)
        return;

    __DoBan(m_actorBans,TARGET_TYPE_ACTOR,name,BAN_TYPE_CHAT,lastTime,reason);
}

void CBanlist::UnBanIpLogin(const char* name)
{
    __DelBan(m_ipBans,name,BAN_TYPE_LOGIN);
}

void CBanlist::UnBanIpChat(const char* name)
{
    __DelBan(m_ipBans,name,BAN_TYPE_CHAT);
}

void CBanlist::UnBanAccLogin(const char* name)
{
    __DelBan(m_accBans,name,BAN_TYPE_LOGIN);
}

void CBanlist::UnBanAccChat(const char* name)
{
    __DelBan(m_accBans,name,BAN_TYPE_CHAT);
}

void CBanlist::UnBanActorLogin(const char* name)
{
    __DelBan(m_actorBans,name,BAN_TYPE_LOGIN);
}

void CBanlist::UnBanActorChat(const char* name)
{
    __DelBan(m_actorBans,name,BAN_TYPE_CHAT);
}

bool CBanlist::IsIpLoginBan(const char* name,BanInfo* pInfo)
{
    return __IsBaned(m_ipBans,name,BAN_TYPE_LOGIN,pInfo);
}

bool CBanlist::IsIpChatBan(const char* name,BanInfo* pInfo)
{
    return __IsBaned(m_ipBans,name,BAN_TYPE_CHAT,pInfo);
}

bool CBanlist::IsAccLoginBan(const char* name,BanInfo* pInfo)
{
    return __IsBaned(m_accBans,name,BAN_TYPE_LOGIN,pInfo);
}

bool CBanlist::IsAccChatBan(const char* name,BanInfo* pInfo)
{
    return __IsBaned(m_accBans,name,BAN_TYPE_CHAT,pInfo);
}

bool CBanlist::IsActorLoginBan(const char* name,BanInfo* pInfo)
{   
    return __IsBaned(m_actorBans,name,BAN_TYPE_LOGIN,pInfo);
}

bool CBanlist::IsActorChatBan(const char* name,BanInfo* pInfo)
{
    return __IsBaned(m_actorBans,name,BAN_TYPE_CHAT,pInfo);
}

void CBanlist::__DoBan(BAN_MAP& banMap,int target,const char* name,int banType,int banTime,const char* reason)
{
    if (0 == name || 0 == reason)
        return;

    name = __TranslateName(name);

    BAN_MAP::iterator iter = banMap.find(name);

    if (iter == banMap.end())
    {
        iter = banMap.insert(std::make_pair(name,BanInfo())).first;
    }

    BanInfo& info = iter->second;

    info.name = name;
    info.type = target;

    //永久冻结帐号，给个足够大的年数
    if (0 == banTime)
    {
        banTime = 622080000; //10年
    }
    
    switch(banType)
    {
    case BAN_TYPE_CHAT:
        {
            info.chatBanExpire = (int)time(0) + banTime;
            info.chatReason    = reason ? reason : "";
        }
        break;
    case BAN_TYPE_LOGIN:
        {
            info.loginBanExpire = (int)time(0) + banTime;
            info.loginReason    = reason ? reason : "";
        }
        break;
    }

    //save to db
    __SaveBanToDB(info);
}

bool CBanlist::__IsBaned(BAN_MAP& banMap,const char* name,int type,BanInfo* pInfo)
{
    if (0 == name)
        return false;

    name = __TranslateName(name);

    BAN_MAP::iterator iter = banMap.find(name);

    if (iter == banMap.end())
        return false;

    BanInfo& info = iter->second;

    if (0 != pInfo)
        *pInfo = info;

    int curTime = (int)time(0);

    switch(type)
    {
    case BAN_TYPE_CHAT:
        {
            return info.IsChatBaned(curTime);
        }
    case BAN_TYPE_LOGIN:
        {
            return info.IsLoginBaned(curTime);
        }
    }

    return false;
}

void CBanlist::__DelBan(BAN_MAP& banMap,const char* name,int type)
{
    if (0 == name)
        return;

    name = __TranslateName(name);

    BAN_MAP::iterator iter = banMap.find(name);

    if (iter != banMap.end())
    {
        BanInfo& info = iter->second;

        switch(type)
        {
        case BAN_TYPE_CHAT:
            {
                info.chatBanExpire = 0;
                info.chatReason    = "";
            }
            break;
        case BAN_TYPE_LOGIN:
            {
                info.loginBanExpire = 0;
                info.loginReason    = "";
            }
            break;
        }

        __SaveBanToDB(info);

        if (!__HasBaned(info))
        {
            banMap.erase(iter);
        }
    }
}

void CBanlist::__SaveBanToDB(BanInfo& info)
{
    CDBOperator dbHandle(DATABASE);

    //首先删除这条记录
    dbHandle->SQL("DELETE FROM banInfo WHERE target='%s' AND type=%d",info.name.c_str(),info.type);
    dbHandle->Eval();

    if (__HasBaned(info))
    {
        //重新插入
        dbHandle->SQL("INSERT INTO banInfo(target,type,loginBan,chatBan,loginBanReason,chatBanReason) VALUES ('%s',%d,%d,%d,'%s','%s')",
            info.name.c_str(),info.type,info.loginBanExpire,info.chatBanExpire,info.loginReason.c_str(),info.chatReason.c_str());

        dbHandle->Eval();
    }
}

const char* CBanlist::__TranslateName(const char* name)
{   
    static char tmp[256];
    strcpy_s(tmp,sizeof(tmp),name);
    _strupr_s(tmp,sizeof(tmp));
    return tmp;
}

bool CBanlist::__HasBaned(BanInfo& info)
{
    int curTime = (int)time(0);

    return info.IsChatBaned(curTime) || info.IsLoginBaned(curTime);
}