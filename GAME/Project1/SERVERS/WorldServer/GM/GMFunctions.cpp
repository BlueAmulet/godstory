#include "GMFunctions.h"
#include "DBLib/dbLib.h"
#include "GMRecver.h"
#include "..\TeamManager.h"
#include "..\ChatMgr.h"
#include "..\PlayerMgr.h"
#include "..\ServerMgr.h"
#include "..\WorldServer.h"
#include "..\CopymapManager.h"
#include "..\MailManager.h"
#include "..\BanList.h"
#include "zlib\zlib.h"
#include "Common\Log\LogTypes.h"
#include <time.h>

//判断玩家是否离线
bool _IsPlayerOffline(U32 playerId)
{
    PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(playerId);

    if (!pPlayer.isNull())
        return false;

    return true;
}

//判断帐号是否离线
bool _IsAccountOffline(U32 accountId)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccount && !pAccount->IsDeleting())
    {
        return false;
    }

    return true;
}

/************************************************************************/
/* 读取一条完整的帐号记录
/************************************************************************/
bool g_ReadAccountInfo(CDBOperator& dbHandle,std::vector<stAccountInfo>& infos)
{
    stAccountInfo accountInfo;

    while(dbHandle->More())
    {
        accountInfo.AccountId   = dbHandle->GetInt();
        strcpy_s(accountInfo.AccountName,sizeof(accountInfo.AccountName),dbHandle->GetString());
        accountInfo.isAdult     = dbHandle->GetInt() ? true : false;
        accountInfo.mode        = dbHandle->GetInt();
        accountInfo.status      = dbHandle->GetInt();
        accountInfo.isGM        = dbHandle->GetInt();
        accountInfo.loginStatus = dbHandle->GetInt();
        accountInfo.loginTime   = dbHandle->GetTime();
        accountInfo.logoutTime  = dbHandle->GetTime();
        accountInfo.loginIP     = dbHandle->GetString();
        int loginError          = dbHandle->GetInt();
        accountInfo.totalOnlineTime = dbHandle->GetInt();
        int accountFunction     = dbHandle->GetInt();
        int pointNum            = dbHandle->GetInt();

        infos.push_back(accountInfo);
    }

    return infos.size() != 0;
}

static const char* g_FormatDate(time_t time)
{
    static char timeBuf[256] = {0};

    tm * pTM = localtime(&time);

    sprintf_s(timeBuf,sizeof(timeBuf),"%d-%d-%d %d:%d:%d",pTM->tm_year + 1900,pTM->tm_mon + 1,pTM->tm_mday,pTM->tm_hour,pTM->tm_min,pTM->tm_sec);
    return timeBuf;
}
  
void GM_AccountQuery(const char* name,int mode)
{
    if (0 == name || 0 == strlen(name))
        return;

    Base::BitStream* pack = CGMRecver::Instance()->GetPack();

    if (0 == pack)
        return;

    CGMAccountQueryFilter filter;
    pack->readBits(sizeof(CGMAccountQueryFilter)* 8,&filter);

    //从数据库查询对应的帐号信息
    std::string nameTmp = "%";
    nameTmp += name;
    nameTmp += "%";

    std::vector<stAccountInfo> accountInfos;

    std::string adultFilter;

    switch(filter.isAdult)
    {
    case 0:
        {
            adultFilter = "IsAdult >= 0";
        }
        break;
    case 1:
        {
            adultFilter = "IsAdult = 0";
        }
        break;
    case 2:
        {   
            adultFilter = "IsAdult > 0";
        }
        break;
    }

    std::string gmFilter;

    switch(filter.isGM)
    {
    case 0:
        {
            gmFilter = "GMFlag >= 0";
        }
        break;
    case 1:
        {
            gmFilter = "GMFlag = 0";
        }
        break;
    case 2:
        {   
            gmFilter = "GMFlag > 0";
        }
        break;
    }

    std::string loginTimeFilter;
    std::string logoutTimeFilter;
    
    if (0 != filter.timeStart)
    {
        loginTimeFilter = g_FormatDate(filter.timeStart);      
    }

    if (0 == filter.timeEnd)
    {
        filter.timeEnd = time(0);
    }

    if (0 != filter.timeEnd)
    {
        logoutTimeFilter = g_FormatDate(filter.timeEnd);
    }

    if (0 == filter.maxQuery)
        filter.maxQuery = 30;

    if(0 == mode || 1 == mode)
    {
        //根据帐号或帐号名
        CDBOperator dbHandle(DATABASE_ACCOUNT);
        dbHandle->SQL("SELECT TOP 500 * FROM BaseInfo WHERE(%s LIKE '%s' AND %s AND %s AND LoginTime>='%s' AND LogoutTime<='%s')",0 == mode ? "AccountName" : "AccountId",nameTmp.c_str(),adultFilter.c_str(),gmFilter.c_str(),loginTimeFilter.c_str(),logoutTimeFilter.c_str());

        if (!g_ReadAccountInfo(dbHandle,accountInfos))
            return;
    }
    else
    {
        //角色名对应的所有帐号ID
        std::vector<int> accountIds;
        CDBOperator dbHandleActor(DATABASE);

        dbHandleActor->SQL("SELECT TOP 500 AccountId FROM PlayerInfo WHERE(%s LIKE '%s')","PlayerName",nameTmp.c_str());

        while(dbHandleActor->More())
        {
            int accountId = dbHandleActor->GetInt();
            accountIds.push_back(accountId);
        }

        if (accountIds.size() == 0)
            return;

        for (size_t i = 0; i < accountIds.size(); ++i)
        {
            CDBOperator dbHandle(DATABASE_ACCOUNT);
            dbHandle->SQL("SELECT TOP 1 * FROM BaseInfo WHERE (AccountId=%d AND %s AND %s AND LoginTime>='%s' AND LogoutTime<='%s')",accountIds[i],adultFilter.c_str(),gmFilter.c_str(),loginTimeFilter.c_str(),logoutTimeFilter.c_str());

            if (!g_ReadAccountInfo(dbHandle,accountInfos))
                return;
        }
    } 

    int queryCount = 0;
        
    for (size_t i = 0; i < accountInfos.size(); ++i)
    {
        if (queryCount > filter.maxQuery)
            break;

        CGMAccountQueryResp resp;

        resp.accountId     = accountInfos[i].AccountId;
        resp.isGM          = accountInfos[i].isGM != 0;
        strncpy_s(resp.accountName,accountInfos[i].AccountName,sizeof(resp.accountName));
        resp.gameZone      = 0;
        resp.status        = accountInfos[i].status;
    
        CBanlist::BanInfo banInfo;
        if (CBanlist::Instance()->IsAccLoginBan(accountInfos[i].AccountName,&banInfo))
        {
            resp.freezeTime   = banInfo.loginBanExpire;
            resp.status      |= ACCOUNT_STATUS_FREEZE;

            strncpy_s(resp.freezeReason,banInfo.loginReason.c_str(),sizeof(resp.freezeReason));
        }
        else
        {
            if (CBanlist::Instance()->IsIpLoginBan(accountInfos[i].loginIP.c_str()))
            {
                resp.freezeTime   = banInfo.loginBanExpire;
                resp.status      |= ACCOUNT_STATUS_IPFREEZE;

                strncpy_s(resp.freezeReason,banInfo.loginReason.c_str(),sizeof(resp.freezeReason));
            }
        }

        if (CBanlist::Instance()->IsAccChatBan(accountInfos[i].AccountName))
        {
            resp.status |= ACCOUNT_STATUS_MUTE;
        }
        
        //判断条件是否过滤
        if (0 != filter.status && !(filter.status & resp.status))
        {
            continue;
        }

        queryCount++;

        CDBOperator dbHandleActor(DATABASE);
        //获取帐号对应的角色名称
        dbHandleActor->SQL("SELECT TOP %d PlayerId,PlayerName FROM PlayerInfo WHERE AccountId=%d",5,accountInfos[i].AccountId);
        
        std::string actorNames[5];
        int iActor = 0;

        while(dbHandleActor->More())
        {
            resp.actorId[iActor] = dbHandleActor->GetInt();
            actorNames[iActor++] = dbHandleActor->GetString();
        }

        for (int iActor = 0; iActor < 5; ++iActor)
        {
            strncpy_s(resp.actor[iActor],actorNames[iActor].c_str(),sizeof(resp.actor[iActor]));
        }
        
        resp.lastLoginTime   = accountInfos[i].loginTime;
        resp.lastLogoutTime  = accountInfos[i].logoutTime;
        strncpy_s(resp.lastLoginIp,accountInfos[i].loginIP.c_str(),sizeof(resp.lastLoginIp));

        CGMRecver::Instance()->PushResp(resp);
    }
}
 
void GM_AccountFreeze(const char* accountName,int freezeTime,int isIPFreeze,const char* reason)
{
    if (0 == accountName || 0 == reason)
        return;

    int error = 0;

    CDBOperator dbHandle(DATABASE_ACCOUNT);
    dbHandle->SQL("SELECT AccountId,LoginIP FROM BaseInfo WHERE AccountName='%s'",accountName);

    if (!dbHandle->More())
        return;

    int accountId = dbHandle->GetInt();

    //如果当前帐号在线,则踢下线
    stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccountInfo)
    {
        pAccountInfo->Kick("",error, true);
    }

    if (0 == error)
    {
        if (!isIPFreeze)
        {
            CBanlist::Instance()->BanAccLogin(accountName,freezeTime,reason);
        }
        else
        {
            std::string ip = dbHandle->GetString();

            if ("" != ip)
            {
                CBanlist::Instance()->BanIpLogin(ip.c_str(),freezeTime,reason);
            }
        }
    }
    
    CGMActorAccountOpResp resp;
    
    strncpy_s(resp.name,accountName,sizeof(resp.name));
    resp.error       = error;

    CGMRecver::Instance()->PushResp(resp);
}
 
void GM_AccountUnFreeze(const char* accountName,const char* reason)
{
    if (0 == accountName || 0 == reason)
        return;

    CBanlist::Instance()->UnBanAccLogin(accountName);

    CDBOperator dbHandle(DATABASE_ACCOUNT);
    dbHandle->SQL("SELECT LoginIP FROM BaseInfo WHERE AccountName='%s'",accountName);

    if (dbHandle->More())
    {
        CBanlist::Instance()->UnBanIpLogin(dbHandle->GetString());
    }

    CGMActorAccountOpResp resp;
    
    strncpy_s(resp.name,accountName,sizeof(resp.name));
    resp.error       = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void GM_AccountChatMute(const char* accountName,int time,const char* reason)
{
    if (0 == accountName || 0 == reason)
        return;

    CBanlist::Instance()->BanAccChat(accountName,time,reason);

    CGMActorAccountOpResp resp;
    strncpy_s(resp.name,accountName,sizeof(resp.name));
    resp.error       = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void GM_AccountChatUnMute(const char* accountName,int time,const char* reason)
{
    if (0 == accountName || 0 == reason)
        return;
    
    CBanlist::Instance()->UnBanAccChat(accountName);

    CGMActorAccountOpResp resp;
    strncpy_s(resp.name,accountName,sizeof(resp.name));
    resp.error       = 0;
    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorChatMute(const char* actorName,int time,const char* reason)
{
    if (0 == actorName || 0 == reason)
        return;

    CBanlist::Instance()->BanActorChat(actorName,time,reason);

    CGMActorAccountOpResp resp;
    strncpy_s(resp.name,actorName,sizeof(resp.name));
    resp.error       = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorChatUnMute(const char* actorName,int time,const char* reason)
{
    if (0 == actorName || 0 == reason)
        return;

    CBanlist::Instance()->UnBanActorChat(actorName);

    CGMActorAccountOpResp resp;
    strncpy_s(resp.name,actorName,sizeof(resp.name));
    resp.error       = 0;
    CGMRecver::Instance()->PushResp(resp);
}

void    GM_ActorFreeze(const char* actorName,int time,const char* reason)
{
    if (0 == actorName || 0 == reason)
        return;

	int error = 0;

    //踢线
    PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(actorName);

    if (!pPlayer.isNull())
    {
		stAccountInfo* pInfo = SERVER->GetPlayerManager()->GetAccountInfo( pPlayer->BaseData.PlayerId );

		if( pInfo )
			pInfo->Kick("",error,true);
    }

    CBanlist::Instance()->BanActorLogin(actorName,time,reason);

    CGMActorAccountOpResp resp;

    strncpy_s(resp.name,actorName,sizeof(resp.name));
    resp.error       = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void    GM_ActorUnFreeze(const char* actorName,const char* reason)
{
    if (0 == actorName || 0 == reason)
        return;
    
    CBanlist::Instance()->UnBanActorLogin(actorName);

    CGMActorAccountOpResp resp;

    strncpy_s(resp.name,actorName,sizeof(resp.name));
    resp.error       = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void GM_Kick(const char* accountName)
{
    if(0 == accountName)
        return;

    int error = 0;

    CDBOperator dbHandle(DATABASE_ACCOUNT);
    dbHandle->SQL("SELECT AccountId FROM BaseInfo WHERE AccountName='%s'",accountName);

    if (!dbHandle->More())
        return;

    int accountId = dbHandle->GetInt();

    stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccountInfo)
    {
        error = pAccountInfo->Kick("",error,true);
    }

    CGMActorAccountOpResp resp;

    strncpy_s(resp.name,accountName,sizeof(resp.name));
    resp.error     = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorKick(const char* actorName)
{
    if (0 == actorName)
        return;

    int error = 0;

    CDBOperator dbHandle(DATABASE);
    dbHandle->SQL("SELECT AccountId FROM PlayerInfo WHERE PlayerName='%s'",actorName);

    if (!dbHandle->More())
        return;

    int accountId = dbHandle->GetInt();

    stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccountInfo)
    {
        error = pAccountInfo->Kick("", error, true);
    }

    CGMActorAccountOpResp resp;

    strncpy_s(resp.name,actorName,sizeof(resp.name));
    resp.error     = 0;

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorBaseInfo(int accountId,const char* actorName)
{
    if (0 == actorName)
        return;

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(actorName,false);
    
    if (0 == pPlayerInfo)
        return;
    
    CGMPlayerBaseInfo resp;

    resp.accountId      = accountId;
    strncpy_s(resp.playerName,pPlayerInfo->BaseData.PlayerName,sizeof(resp.playerName));
    resp.hp             = pPlayerInfo->MainData.CurrentHP;
    resp.mp             = pPlayerInfo->MainData.CurrentMP;
    resp.playerHot      = pPlayerInfo->MainData.playerHot;
    resp.CurrentPP      = pPlayerInfo->MainData.CurrentPP;
    resp.CurrentVigor   = pPlayerInfo->MainData.CurrentVigor;

    resp.freezeTime     = -1;
    resp.muteTime       = -1;
    
    CBanlist::BanInfo banInfo;
    if (CBanlist::Instance()->IsActorLoginBan(resp.playerName,&banInfo))
    {
        resp.freezeTime = banInfo.loginBanExpire;
    }

    if (CBanlist::Instance()->IsActorChatBan(resp.playerName,&banInfo))
    {
        resp.muteTime = banInfo.chatBanExpire;
    }

    resp.zone           = pPlayerInfo->DispData.ZoneId;
    resp.playerId       = pPlayerInfo->BaseData.PlayerId;
    resp.sex            = pPlayerInfo->DispData.Sex;
    resp.level          = pPlayerInfo->DispData.Level;
    resp.status         = 0;
    resp.isOnline       = (0 != SERVER->GetPlayerManager()->GetAccountInfo(pPlayerInfo->BaseData.PlayerId));
    resp.gold           = pPlayerInfo->MainData.Money;
    resp.exp            = pPlayerInfo->MainData.CurrentExp;
    resp.loginCount     = pPlayerInfo->DispData.PlayerOnlineTime;
    resp.Race           = pPlayerInfo->DispData.Race;
    resp.Family         = pPlayerInfo->DispData.Family;

    memcpy(resp.Classes,pPlayerInfo->DispData.Classes,sizeof(resp.Classes));

    resp.createTime     = pPlayerInfo->BaseData.createTime;
    resp.lastLoginTime  = pPlayerInfo->BaseData.lastLoginTime;
    resp.lastLogoutTime = pPlayerInfo->BaseData.lastLogoutTime;

    strncpy_s(resp.LeavePos,pPlayerInfo->DispData.LeavePos,sizeof(resp.LeavePos));

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorDeletedBaseInfo(int accountId,const char* actorName)
{
    if (0 == actorName)
        return;

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(actorName,true);

    if (0 == pPlayerInfo)
        return;

    CGMPlayerBaseInfo resp;

    resp.accountId      = accountId;
    strncpy_s(resp.playerName,pPlayerInfo->BaseData.PlayerName,sizeof(resp.playerName));
    resp.zone           = pPlayerInfo->DispData.ZoneId;
    resp.playerId       = pPlayerInfo->BaseData.PlayerId;
    resp.sex            = pPlayerInfo->DispData.Sex;
    resp.level          = pPlayerInfo->DispData.Level;
    resp.status         = 0;
    resp.isOnline       = false;
    resp.gold           = pPlayerInfo->MainData.Money;
    resp.exp            = pPlayerInfo->MainData.CurrentExp;
    resp.loginCount     = pPlayerInfo->DispData.PlayerOnlineTime;
    resp.Race           = pPlayerInfo->DispData.Race;
    resp.Family         = pPlayerInfo->DispData.Family;

    memcpy(resp.Classes,pPlayerInfo->DispData.Classes,sizeof(resp.Classes));

    resp.createTime     = pPlayerInfo->BaseData.createTime;
    resp.lastLoginTime  = pPlayerInfo->BaseData.lastLoginTime;
    resp.lastLogoutTime = pPlayerInfo->BaseData.lastLogoutTime;
    strncpy_s(resp.LeavePos,pPlayerInfo->DispData.LeavePos,sizeof(resp.LeavePos));

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorSkill(int playerId)
{   
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        unsigned int skillId = pPlayerInfo->MainData.SkillInfo[i].SkillId;

        if (0 == skillId)
            continue;

        CGMPlayerSkillInfoResp resp;
        resp.playerId = playerId;
        resp.skillId  = skillId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorSkillAdd(int playerId,unsigned int skillId)
{
    if (0 == skillId || !_IsPlayerOffline(playerId))
    {
        return;
    }

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    bool isFound = false;

    for (int i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.SkillInfo[i].SkillId == skillId)
        {
            isFound = true;
            break;
        }
    }

    if (isFound)
        return;

    int i = 0;

    for (i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.SkillInfo[i].SkillId == 0)
        {
            pPlayerInfo->MainData.SkillInfo[i].SkillId = skillId;
            break;
        }
    }

    //保存
    if (i < SKILL_MAXSLOTS && NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerSkillOpResp resp;
        resp.playerId = playerId;
        resp.skillId  = skillId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorSkillDel(int playerId,unsigned int skillId)
{
    if (0 == skillId || !_IsPlayerOffline(playerId))
    {
        return;
    }

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    if (0 == skillId)
    {
        return;
    }

    int i = 0;

    for (i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.SkillInfo[i].SkillId == skillId)
        {
            pPlayerInfo->MainData.SkillInfo[i].SkillId = 0;
            break;
        }
    }

    if (i >= SKILL_MAXSLOTS)
        return;

    //保存
    if (NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerSkillOpResp resp;
        resp.playerId = playerId;
        resp.skillId  = skillId;
        CGMRecver::Instance()->PushResp(resp);
    }
} 

void GM_ActorSkillMdf(int playerId,unsigned int oldId,unsigned int newSkillId)
{
    if (0 == newSkillId || !_IsPlayerOffline(playerId))
    {
        return;
    }

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    if (0 == oldId || 0 == newSkillId)
    {
        return;
    }

    int i = 0;

    for (i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.SkillInfo[i].SkillId == newSkillId)
        {
            break;
        }
    }

    if (i < SKILL_MAXSLOTS)
        return;

    for (i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.SkillInfo[i].SkillId == oldId)
        {
            pPlayerInfo->MainData.SkillInfo[i].SkillId = newSkillId;
            break;
        }
    }

    if (i >= SKILL_MAXSLOTS)
        return;

    //保存
    if (NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerSkillOpResp resp;
        resp.playerId = playerId;
        resp.skillId  = newSkillId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorLivingSkill(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < SKILL_MAXSLOTS; ++i)
    {
        unsigned int skillId = pPlayerInfo->MainData.LivingSkillInfo[i].SkillID;

        if (0 == skillId)
            continue;

        CGMPlayerLivingSkillResp resp;
        resp.playerId = playerId;
        resp.skillId  = skillId;
        resp.Ripe     = pPlayerInfo->MainData.LivingSkillInfo[i].Ripe;

        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorQuestFinished(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    U8* pState = pPlayerInfo->MainData.MissionInfo.State;

    for(S32 i = 0; i < MISSION_MAX; ++i)
    {
        if ((pState[i >> 3] & U8(1 << (i & 0x7))) != 0)
        {
            CGMPlayerQuestFinishedInfoResp resp;

            resp.playerId = playerId;
            resp.questId  = i;

            CGMRecver::Instance()->PushResp(resp);
        }
    }   
}

void GM_ActorQuestCycle(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < MISSION_CYCLE_MAX; ++i)
    {
        if (pPlayerInfo->MainData.MissionInfo.Cycle[i].MissionID <= 0)
            continue;

        CGMPlayerQuestCycleInfoResp resp;
        resp.playerId = playerId;
        memcpy(&resp.info,&pPlayerInfo->MainData.MissionInfo.Cycle[i],sizeof(resp.info));
        
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorQuestAccepted(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < MISSION_ACCEPT_MAX; ++i)
    {
        if (pPlayerInfo->MainData.MissionInfo.Flag[i].MissionID <= 0)
            continue;

        CGMPlayerQuestAcceptInfoResp resp;
        resp.playerId = playerId;
        memcpy(&resp.info,&pPlayerInfo->MainData.MissionInfo.Flag[i],sizeof(resp.info));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorFriend(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < SOCAIL_ITEM_MAX; ++i)
    {
        stSocialItem& item = pPlayerInfo->MainData.SocialItem[i];

        if (0 == item.playerId)
            continue;

        CDBOperator dbHandle(DATABASE);
        dbHandle->SQL("SELECT PlayerName FROM PlayerInfo WHERE PlayerId=%d",item.playerId);

        if (!dbHandle->More())
            continue;

        CGMPlayerFriendInfoResp resp;

        resp.playerId    = playerId;
        resp.type        = item.type;
        strncpy_s(resp.friendName,dbHandle->GetString(),sizeof(resp.friendName));
        resp.friendValue = item.friendValue;
        resp.isOnline    = (0 != SERVER->GetPlayerManager()->GetAccountInfo(item.playerId));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void	GM_ActorRide(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < MOUNTPET_MAXSLOTS; ++i)
    {
        stMountInfo& item = pPlayerInfo->MainData.MountInfo[i];

        if (0 == item.mDataId)
            continue;

        CGMPlayerRideInfoResp resp;

        resp.playerId    = playerId;
        memcpy(&resp.info,&item,sizeof(item));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorMail(int accountId,int actorId)
{
    
}

/************************************************************************/
/* 简单的物品信息
/************************************************************************/
void GM_ActorItem(int accountId,int playerId,int slot)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    switch(slot)
    {
    case 0:
        {
            //物品栏
            for (int i = 0; i < INVENTROY_MAXSLOTS; ++i)
            {   
                stItemInfo& item = pPlayerInfo->MainData.InventoryInfo[i];

                if (item.UID != 0)
                {
                    CGMPlayerItemBaseInfoResp resp;

                    resp.playerId = playerId;
                    resp.uid      = item.UID;
                    resp.itemID   = item.ItemID;
                    resp.slot     = slot;

                    CGMRecver::Instance()->PushResp(resp);
                }
            }
        }
        break;
    case 1:
        {
            //银行
            for (int i = 0; i < BANK_MAXSLOTS; ++i)
            {   
                stItemInfo& item = pPlayerInfo->MainData.BankInfo[i];

                if (item.UID != 0)
                {
                    CGMPlayerItemBaseInfoResp resp;

                    resp.playerId = playerId;
                    resp.uid      = item.UID;
                    resp.itemID   = item.ItemID;
                    resp.slot     = slot;

                    CGMRecver::Instance()->PushResp(resp);
                }
            }
        }
        break;
    case 2:
        {
            //装备栏
            for (int i = 0; i < EQUIP_MAXSLOTS; ++i)
            {   
                stItemInfo& item = pPlayerInfo->DispData.EquipInfo[i];

                if (item.UID != 0)
                {
                    CGMPlayerItemBaseInfoResp resp;

                    resp.playerId = playerId;
                    resp.uid      = item.UID;
                    resp.itemID   = item.ItemID;
                    resp.slot     = slot;

                    CGMRecver::Instance()->PushResp(resp);
                }
            }
        }
        break;
    }
}

stItemInfo* _GetItemFromPlayer(stPlayerStruct& playerData,U64 itemUID)
{
    int i = 0;

    //查找对应的物品
    for (i = 0; i < INVENTROY_MAXSLOTS; ++i)
    {   
        stItemInfo& item = playerData.MainData.InventoryInfo[i];

        if (item.UID == itemUID)
        {
            return &playerData.MainData.InventoryInfo[i];
        }
    }

    //装备栏
    for (i = 0; i < EQUIP_MAXSLOTS; ++i)
    {   
        stItemInfo& item = playerData.DispData.EquipInfo[i];

        if (item.UID == itemUID)
        {
            return &playerData.DispData.EquipInfo[i];
        }
    }

    for (i = 0; i < BANK_MAXSLOTS; ++i)
    {   
        stItemInfo& item = playerData.MainData.BankInfo[i];

        if (item.UID == itemUID)
        {
            return &playerData.MainData.BankInfo[i];
        }
    }

    return 0;
}

stItemInfo* _GetEmptyItemSlotFromPlayer(stPlayerStruct& playerData)
{
    for (int i = 0; i < INVENTROY_MAXSLOTS; ++i)
    {   
        stItemInfo& item = playerData.MainData.InventoryInfo[i];

        if (item.UID == 0)
        {
            return &playerData.MainData.InventoryInfo[i];
        }
    }

    return 0;
}

void GM_ActorItemDetail(int playerId,const char* strItemUID)
{
    U64 itemUID = _atoi64(strItemUID);

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    stItemInfo* item = _GetItemFromPlayer(*pPlayerInfo,itemUID);

    if (0 == item)
    {
        return;
    }

    //返回信息
    CGMPlayerItemDetailInfoResp resp;
    
    strncpy_s(resp.Producer,item->Producer,sizeof(resp.Producer));   // 制造者名称
    resp.UID                = item->UID;				    // 物品世界唯一ID
    resp.ItemID             = item->ItemID;					// 物品模板ID
    resp.Quantity           = item->Quality;			    // 物品数量(对于装备，只能为1)
    resp.LapseTime          = item->LapseTime;				// 剩余秒数
    resp.RemainUseTimes     = item->RemainUseTimes;			// 剩余使用次数
    resp.BindPlayer         = item->BindPlayer;				// 绑定者ID(也可用于存储灵魂链接开启后玩家ID)
    resp.Quality            = item->Quality;			    // 物品品质等级
    resp.CurWear            = item->CurWear;			    // 当前耐久度
    resp.CurMaxWear         = item->CurMaxWear;				// 当前最大耐久度
    resp.CurAdroit          = item->CurAdroit;				// 当前熟练度
    resp.CurAdroitLv        = item->CurAdroitLv;		    // 当前熟练度等级
    resp.ActivatePro        = item->ActivatePro;		    // 效果激活标志
    resp.RandPropertyID     = item->RandPropertyID;			// 是否随机名称属性
    resp.IDEProNum          = item->IDEProNum;				// 鉴定后附加属性个数

    for (int i = 0; i < MAX_IDEPROS; ++i)
    {
        resp.IDEProValue[i] = item->IDEProValue[i];			// 鉴定后附加属性值
    }
    
    resp.EquipStrengthens = item->EquipStrengthens;			// 装备强化过总数
    
    for (int i = 0; i < MAX_EQUIPSTENGTHENS; ++i)
    {
        resp.EquipStrengthenValue[i][0] = item->EquipStrengthenValue[i][0];
        resp.EquipStrengthenValue[i][1] = item->EquipStrengthenValue[i][1];
    }
    
    resp.ImpressID          = item->ImpressID;			// 铭刻ID(状态ID)
    resp.BindProID          = item->BindProID;			// 灵魂绑定附加属性(状态ID)
    resp.WuXingID           = item->WuXingID;			// 五行ID
    resp.WuXingLinkID       = item->WuXingLinkID;		// 五行链接ID
    resp.WuXingPro          = item->WuXingPro;			// 五行效果属性
    resp.SkillAForEquip     = item->SkillAForEquip;		// 装备上获取的主动技能ID
    resp.SkillBForEquip     = item->SkillBForEquip;		// 装备上获取的被动技能ID
    resp.EmbedOpened        = item->EmbedOpened;		// 开启镶嵌孔（每4bit位代表一个孔）

    for(int i = 0; i < MAX_EMBEDSLOTS; ++i)
    {
        resp.EmbedSlot[i] = item->EmbedSlot[i];// 装备镶嵌孔的宝石ID
    }

    CGMRecver::Instance()->PushResp(resp);
} 

void GM_ActorTransport(int playerId,int mapId,float x,float y,float z)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo(playerId);

    if (0 == pAccount)
        return;

    pAccount->TransportPlayer(playerId,mapId,x,y,z);

    CGMPlayerTransportResp resp;
    resp.playerId = playerId;
    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorLevelMdf(int accountId,int playerId,int newLevel)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccount && !pAccount->IsDeleting())
    {
        CGMRecver::Instance()->SetError(GM_ERROR_ACCOUNT_ONLINE);
        return;
    }

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    pPlayerInfo->DispData.Level = newLevel;

    //保存
    if (NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerBaseInfoMdfResp resp;
        resp.playerId = playerId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorExpMdf(int accountId,int playerId,int newExp)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccount && !pAccount->IsDeleting())
    {
        CGMRecver::Instance()->SetError(GM_ERROR_ACCOUNT_ONLINE);
        return;
    }

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    pPlayerInfo->MainData.CurrentExp = newExp;

    //保存
    if (NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerBaseInfoMdfResp resp;
        resp.playerId = playerId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorGoldMdf(int accountId,int playerId,int newGold)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if (0 != pAccount && !pAccount->IsDeleting())
    {
        CGMRecver::Instance()->SetError(GM_ERROR_ACCOUNT_ONLINE);
        return;
    }

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    pPlayerInfo->MainData.Money = newGold;

    //保存
    if (NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerBaseInfoMdfResp resp;
        resp.playerId = playerId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorItemModify(int playerId,const char* UID)
{
    if (!_IsPlayerOffline(playerId))
        return;

    U64 itemUID = _atoi64(UID);

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    Base::BitStream* pack = CGMRecver::Instance()->GetPack();

    if (0 == pack)
        return;

    CGMPlayerItemDetailInfoResp itemData;
    itemData.unpack(*pack);

    stItemInfo* item = _GetItemFromPlayer(*pPlayerInfo,itemUID);

    if (0 == item)
    {
        item = _GetEmptyItemSlotFromPlayer(*pPlayerInfo);

        if (0 == item)
            return;
    }

    //修改并保存当前玩家数据
    strncpy_s(item->Producer,itemData.Producer,sizeof(item->Producer));   // 制造者名称
    item->UID                = itemData.UID;				    // 物品世界唯一ID
    item->ItemID             = itemData.ItemID;					// 物品模板ID
    item->Quantity           = itemData.Quality;			    // 物品数量(对于装备，只能为1)
    item->LapseTime          = itemData.LapseTime;				// 剩余秒数
    item->RemainUseTimes     = itemData.RemainUseTimes;			// 剩余使用次数
    item->BindPlayer         = itemData.BindPlayer;				// 绑定者ID(也可用于存储灵魂链接开启后玩家ID)
    item->Quality            = itemData.Quality;			    // 物品品质等级
    item->CurWear            = itemData.CurWear;			    // 当前耐久度
    item->CurMaxWear         = itemData.CurMaxWear;				// 当前最大耐久度
    item->CurAdroit          = itemData.CurAdroit;				// 当前熟练度
    item->CurAdroitLv        = itemData.CurAdroitLv;		    // 当前熟练度等级
    item->ActivatePro        = itemData.ActivatePro;		    // 效果激活标志
    item->RandPropertyID     = itemData.RandPropertyID;			// 是否随机名称属性
    item->IDEProNum          = itemData.IDEProNum;				// 鉴定后附加属性个数

    for (int i = 0; i < MAX_IDEPROS; ++i)
    {
        item->IDEProValue[i] = itemData.IDEProValue[i];  // 鉴定后附加属性值
    }

    item->EquipStrengthens = itemData.EquipStrengthens;		// 装备强化过总数

    for (int i = 0; i < MAX_EQUIPSTENGTHENS; ++i)
    {
        item->EquipStrengthenValue[i][0] = itemData.EquipStrengthenValue[i][0];
        item->EquipStrengthenValue[i][1] = itemData.EquipStrengthenValue[i][1];
    }

    item->ImpressID          = itemData.ImpressID;			// 铭刻ID(状态ID)
    item->BindProID          = itemData.BindProID;			// 灵魂绑定附加属性(状态ID)
    item->WuXingID           = itemData.WuXingID;			// 五行ID
    item->WuXingLinkID       = itemData.WuXingLinkID;		// 五行链接ID
    item->WuXingPro          = itemData.WuXingPro;			// 五行效果属性
    item->SkillAForEquip     = itemData.SkillAForEquip;		// 装备上获取的主动技能ID
    item->SkillBForEquip     = itemData.SkillBForEquip;		// 装备上获取的被动技能ID
    item->EmbedOpened        = itemData.EmbedOpened;		    // 开启镶嵌孔（每4bit位代表一个孔）

    for(int i = 0; i < MAX_EMBEDSLOTS; ++i)
    {
        item->EmbedSlot[i] = itemData.EmbedSlot[i];// 装备镶嵌孔的宝石ID
    }

    //保存
    if (NONE_ERROR == SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true))
    {
        CGMPlayerItemModifyResp resp;

        resp.playerId = playerId;
        resp.itemUID  = itemUID;
        resp.error    = 0;

        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorItemDel(int playerId,const char* UID)
{
    if (!_IsPlayerOffline(playerId))
        return;

    U64 itemUID = _atoi64(UID);

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    stItemInfo* item = _GetItemFromPlayer(*pPlayerInfo,itemUID);

    if (0 == item)
        return;

    item->InitData();

    CGMPlayerItemDelResp resp;

    resp.playerId = playerId;
    resp.itemUID  = itemUID;
    resp.error    = SERVER->GetPlayerManager()->SavePlayerData(pPlayerInfo,true);

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorPos(int accountId,int playerId)
{
    //判断玩家是否在线
    PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(playerId);

    if (pPlayer.isNull() )
        return;

    CGMActorPosResp resp;
    
    resp.playerId = playerId;
    CGMRecver::Instance()->PushResp(resp);

	stAccountInfo* pInfo = SERVER->GetPlayerManager()->GetAccountInfo( playerId );
    
    //刷新位置
    stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServerByZoneId(pInfo->LineId,pPlayer->DispData.ZoneId);

    if (0 == pServerInfo)
        return;

    CMemGuard buf(40);
    Base::BitStream sendPacket( buf.get(),40);

    stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_GetPlayerPosRequest,playerId,SERVICE_ZONESERVER );
    pSendHead->DestZoneId = pPlayer->DispData.ZoneId;
    pSendHead->SrcZoneId  = pPlayer->BaseData.AccountId;
    pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

    SERVER->GetServerSocket()->Send( pServerInfo->SocketId, sendPacket);
}

void GM_ActorRename(int playerId,const char* newName)
{
    if (!_IsPlayerOffline(playerId))
        return;

    if (0 == newName)
        return;

    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    //需要玩家离线状态下操作
    PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(playerId);

    if (pPlayer.isNull())
        return;

    //检测是否有重名的玩家
    CDBOperator dbHandle(DATABASE);
    dbHandle->SQL("SELECT PlayerName FROM PlayerInfo WHERE PlayerName='%s'",newName);

    if (dbHandle->More())
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 != pPlayerInfo)
    {
        strncpy_s(pPlayerInfo->BaseData.PlayerName,newName,sizeof(pPlayerInfo->BaseData.PlayerName));
    }

    //修改玩家名称
    dbHandle->SQL("UPDATE PlayerInfo SET PlayerName='%s' WHERE PlayerId=%d",newName,playerId);

    dbHandle->Exec();

    CGMActorRenameResp resp;

    resp.playerId = playerId;
    strncpy_s(resp.newName,newName,sizeof(resp.newName));

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ActorPet(int playerId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < PET_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.PetInfo[i].id == 0)
            continue;

        CGMPlayerPetResp resp;
        resp.playerId  = playerId;
        resp.petId     = pPlayerInfo->MainData.PetInfo[i].id;
        resp.petDataId = pPlayerInfo->MainData.PetInfo[i].petDataId;

        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorPetInfo(int playerId,int petId)
{
    CGMUser* pGM = CGMRecver::Instance()->GetGM();

    if (0 == pGM)
        return;

    stPlayerStruct* pPlayerInfo = pGM->GetPlayerInfo(playerId);

    if (0 == pPlayerInfo)
        return;

    for (int i = 0; i < PET_MAXSLOTS; ++i)
    {
        if (pPlayerInfo->MainData.PetInfo[i].id == 0 ||
            pPlayerInfo->MainData.PetInfo[i].id != petId)
            continue;

        CGMPlayerPetDetailInfoResp resp;
        resp.playerId  = playerId;
        memcpy(&resp.petInfo,&pPlayerInfo->MainData.PetInfo[i],sizeof(resp.petInfo));

        CGMRecver::Instance()->PushResp(resp);
        break;
    }
}

void    GM_GMRole(int gmFlag)
{
    //对于总管理员默认拥有全部权限
    if (1 == gmFlag)
        return;

    CGMAuthManager& authMgr = CGMRecver::Instance()->GetAuthMgr();
    CGMAuthManager::AUTHS_MAP auths = authMgr.GetAuths(gmFlag);

    for (size_t i = 0; i < auths.size(); ++i)
    {
        CGMRoleResp resp;

        resp.gmFlag = gmFlag;
        strncpy_s(resp.role,auths[i].c_str(),sizeof(resp.role));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void    GM_GMAddRole(int gmFlag,const char* role)
{
    if (0 == role)
        return;

    CGMAuthManager& authMgr = CGMRecver::Instance()->GetAuthMgr();

    if (authMgr.AddAuth(gmFlag,role))
    {
        CGMAddRoleResp resp;

        resp.gmFlag = gmFlag;
        strncpy_s(resp.role,role,sizeof(resp.role));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void    GM_GMDelRole(int gmFlag,const char* role)
{
    if (0 == role)
        return;

    CGMAuthManager& authMgr = CGMRecver::Instance()->GetAuthMgr();

    if (authMgr.DelAuth(gmFlag,role))
    {
        CGMDelRoleResp resp;

        resp.gmFlag = gmFlag;
        strncpy_s(resp.role,role,sizeof(resp.role));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void    GM_GMQueryAccount(int gmFlag)
{
    CDBOperator dbHandle(DATABASE_ACCOUNT);
    dbHandle->SQL("SELECT * FROM BaseInfo WHERE GMFlag=%d",gmFlag);

    while(dbHandle->More())
    {
        int accountId           = dbHandle->GetInt();
        std::string accountName = dbHandle->GetString();

        CGMQueryResp resp;

        resp.accountId   = accountId;
        strncpy_s(resp.accountName,accountName.c_str(),sizeof(resp.accountName));

        CGMRecver::Instance()->PushResp(resp);
    }
}

void    GM_GMMdfAccount(const char* accountName,int gmFlag)
{
    if (0 == accountName)
        return;

    CDBOperator dbHandle(DATABASE_ACCOUNT);
    dbHandle->SQL("UPDATE BaseInfo SET GMFlag=%d WHERE AccountName='%s'",gmFlag,accountName);
    dbHandle->Exec();

    CGMMdfRolResp resp;
    
    resp.gmFlag   = gmFlag;
    strncpy_s(resp.accountName,accountName,sizeof(resp.accountName));

    CGMRecver::Instance()->PushResp(resp);
}

void GM_ChangePassword(const char* accountName,const char* oldPassword,const char* newPassword)
{
    if (0 == accountName || 0 == oldPassword || 0 == newPassword)
        return;

    //CDBOperator dbHandle(DATABASE_ACCOUNT);
    //dbHandle->SQL("UPDATE BaseInfo SET WHERE AccountName='s' AND gmFlag!=0",accountName);

    //TODO修改GM的密码

    CGMPasswordChangeResp resp;
    CGMRecver::Instance()->PushResp(resp);
}
 
void GM_Chat(const char* playerName,int channel)
{
    if (0 == playerName)
        return;
    
    int len = (int)strlen(playerName);
    
    Base::BitStream* pack = CGMRecver::Instance()->GetPack();

    if (0 == pack)
        return;

    stChatMessage msg;

    msg.btMessageType = (len == 0 ? CHAT_MSG_TYPE_SYSTEM : CHAT_MSG_TYPE_GM);
    strncpy_s(msg.szSenderName,"gm",sizeof(msg.szSenderName));
    pack->readLongString(sizeof(msg.szMessage),msg.szMessage);
    
    if (0 == len)
    {
        SERVER->GetChatManager()->SendMessageToWorld(msg);
    }
    else
    {
        PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(playerName);

        if (pPlayer.isNull())
        {
            CGMRecver::Instance()->SetError(GM_ERROR_NOPLAYER);
            return;
        }

        int playerId = pPlayer->BaseData.PlayerId;
        
        switch(channel)
        {
        case CHAT_TARGET_PERSON:
            {
                msg.btMessageType = CHAT_MSG_TYPE_PRIVATE;
                SERVER->GetChatManager()->SendMessageTo(msg,playerId);
            }
            break;
        case CHAT_TARGET_TEAM:
            {
                //获取玩家所在的team
                CTeam* pTeam = SERVER->GetTeamManager()->GetTeamByPlayer(playerId);

                if (0 == pTeam)
                    return;
                
                msg.btMessageType = CHAT_MSG_TYPE_TEAM;
                SERVER->GetChatManager()->SendMessageToChannel(msg,pTeam->GetChatChannelId());
            }   
            break;
        }
    }

    CGMChatResp resp;
    CGMRecver::Instance()->PushResp(resp);
}

void GM_ChatAuto(const char* playerName,int channel,int isSysMsg)
{
    if (0 == playerName)
        return;

    int len = (int)strlen(playerName);
    CChatManager::AutoMsg autoMsg;

    if (0 == len)
        autoMsg.playerId = 0;
    else
    {
        PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData(playerName);

        if (pPlayer.isNull())
        {
            CGMRecver::Instance()->SetError(GM_ERROR_NOPLAYER);
            return;
        }

        autoMsg.playerId = pPlayer->BaseData.PlayerId;
    }

    Base::BitStream* pack = CGMRecver::Instance()->GetPack();

    if (0 == pack)
        return;

    autoMsg.msg.btMessageType = (len == 0 ? CHAT_MSG_TYPE_SYSTEM : CHAT_MSG_TYPE_GM);
    strncpy_s(autoMsg.msg.szSenderName,"gm",sizeof(autoMsg.msg.szSenderName));
    pack->readLongString(sizeof(autoMsg.msg.szMessage),autoMsg.msg.szMessage);

    pack->readBits(sizeof(autoMsg.startTime) * 8,&autoMsg.startTime);
    pack->readBits(sizeof(autoMsg.endTime) * 8,&autoMsg.endTime);
    autoMsg.freq    = pack->readInt(Base::Bit32);
    autoMsg.times   = pack->readInt(Base::Bit32);
    autoMsg.channel = channel;
    
    SERVER->GetChatManager()->AddAutoMsg(autoMsg);

    CGMChatResp resp;
    CGMRecver::Instance()->PushResp(resp);
}

void GM_QueryDelActor(int accountId)
{
    CDBOperator dbHandle(DATABASE);
    dbHandle->SQL("SELECT PlayerId,PlayerName FROM PlayerInfo_Deleted WHERE AccountId=%d",accountId);

    while(dbHandle->More())
    {
        CGMQueryDelActorResp resp;

        resp.accountId = accountId;
        resp.playerId  = dbHandle->GetInt();
        strncpy_s(resp.playerName,dbHandle->GetString(),sizeof(resp.playerName));

        CGMRecver::Instance()->PushResp(resp);
    }
}
 
void GM_ActorDelete(int accountId,int playerId)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(accountId);
    
    int error = UNKNOW_ERROR;

    if (0 != pAccount)
    {
        error = SERVER->GetPlayerManager()->DeletePlayer(pAccount->UID,accountId,playerId);
    }
    else
    {
        CDBOperator dbHandle(DATABASE);

        try
        {
            dbHandle->SQL("EXECUTE DeletePlayer %d",playerId);

            if(dbHandle->More())
            {
                int retCode = dbHandle->GetInt();

                switch(retCode)
                {
                case 0:
                    error = NONE_ERROR;
                    break;
                }
            }
        }
        DB_CATCH_LOG(DeletePlayer);
    }

    if (NONE_ERROR == error)
    {
        CGMDelActorResp resp;
        resp.playerId = playerId;
        CGMRecver::Instance()->PushResp(resp);
    }
}

void GM_ActorUnDelete(int accountId,int playerId)
{
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountMap(accountId);

    if(0 != pAccount)
    {
        CGMRecver::Instance()->SetError(GM_ERROR_ACCOUNT_ONLINE);
        return;
    }

    CDBOperator dbHandle(DATABASE);

    //查询帐号对应的玩家数目
    dbHandle->SQL("SELECT Count(playerId) FROM PlayerInfo WHERE AccountId=%d",accountId);

    if (!dbHandle->More())
        return;

    int playerCount = dbHandle->GetInt();

    if (playerCount + 1 > 5)
    {
        CGMRecver::Instance()->SetError(GM_ERROR_TOOMUCHPLAYER);
        return;
    }

    int error = UNKNOW_ERROR;

    try
    {
        dbHandle->SQL("EXECUTE UnDeletePlayer %d",playerId);

        if(dbHandle->More())
        {
            int retCode = dbHandle->GetInt();

            switch(retCode)
            {
            case 0:
                error = NONE_ERROR;
                break;
            }
        }
    }
    DB_CATCH_LOG(DeletePlayer);

    if (NONE_ERROR == error)
    {
        CGMUnDelActorResp resp;
        resp.playerId = playerId;
        CGMRecver::Instance()->PushResp(resp);
    }
}
