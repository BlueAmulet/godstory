#include "DBLib/dbLib.h"
#include "GMRecver.h"
#include "..\WorldServer.h"
#include "Common\CommonPacket.h"
#include "wintcp\dtServerSocket.h"
#include "..\WorldServer.h"
#include <iostream>
#include "..\PlayerMgr.h"
#include "zlib\zlib.h"

int  tolua_GMExport_open (lua_State* tolua_S);


bool CGMRecver::Initialize()
{
	//初始化gm脚本
	tolua_GMExport_open(m_lua.GetLuaState());

    //读入gm的权限
    m_auth.Load();
    return true;
}

const char* CGMRecver::GetGMName(int accountId)
{
    return "";
}

bool  CGMRecver::HandleCommand(int sessionId,int SocketHandle,int accountId,char gmFlag,const char* fun,Base::BitStream* pack)
{
    CLocker guard(&m_cs);

    m_curSocketHandle = SocketHandle;
    m_curAccountId    = accountId;
    m_curGMFlag       = gmFlag;
    m_curPack         = pack;
    m_error           = 0;

    if (0 == fun)
        return false;

    std::string tmp = fun;
    std::string cmdName = tmp.substr(0,tmp.find("("));
    m_curCMD  = cmdName;

    if (gmFlag != 1 && !m_auth.HasAuth(accountId,cmdName.c_str()))
        m_error = GM_ERROR_AUTH;
    
    if (m_error == GM_ERROR_NONE)
    {        
        try 
        {
            int error = m_lua.DoString(fun);

            if (0 != error)
                m_error = GM_ERROR_CMD;

        }
        catch(...)
        {
            m_error = GM_ERROR_CMD;
        }
        
        if (m_error != GM_ERROR_NONE)
        {
            std::cout << "unable to run gm command:" << fun << " error:" << m_error << std::endl;
        }
    }

    //返回信息给客户端
    static char cmd[4096];

    if (m_resps.size() == 0 || 0 != m_error)
    {
        Base::BitStream pkt(cmd,sizeof(cmd));
        stPacketHead *pSendHead = IPacket::BuildPacketHead(pkt,GM_MESSAGE_RESP,accountId,SERVICE_CLIENT,sessionId);

        pkt.writeString(cmdName.c_str());
        pkt.writeInt(m_error,Base::Bit32);
        pkt.writeInt(0,Base::Bit32);

        pSendHead->PacketSize = pkt.getPosition() - IPacket::GetHeadSize();
        SERVER->GetServerSocket()->Send(SocketHandle,pkt);
    }
    else
    {
        for (size_t i = 0; i < m_resps.size(); ++i)
        {
            Base::BitStream pkt(cmd,sizeof(cmd));
            stPacketHead *pSendHead = IPacket::BuildPacketHead(pkt,GM_MESSAGE_RESP,accountId,SERVICE_CLIENT,sessionId);

            pkt.writeString(cmdName.c_str());
            pkt.writeInt(m_error,Base::Bit32);

            pkt.writeInt(1,Base::Bit32);
            m_resps[i]->pack(pkt);

            pSendHead->PacketSize = pkt.getPosition() - IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(SocketHandle,pkt);
        }
    }

    for (size_t i = 0; i < m_resps.size(); ++i)
    {
        delete m_resps[i];
    }

    m_resps.clear();

    return true;
}

CGMUser* CGMRecver::GetGM(int accountId)
{
    for (size_t i = 0; i < m_gms.size(); ++i)
    {
        if (m_gms[i].GetAccountId() == accountId)
            return &m_gms[i];
    }

    m_gms.push_back(CGMUser(accountId));
    return &m_gms[m_gms.size() - 1];
}

stPlayerStruct* CGMUser::GetPlayerInfo(int playerId,bool isPlayerDeleted)
{
    m_isPlayerDeleted = isPlayerDeleted;

    //判断玩家是否在线
    stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo(playerId);

    if (0 == pAccount)
    {
        if (m_playerInfo.BaseData.PlayerId == playerId)
            return &m_playerInfo;

        const char* dbName = isPlayerDeleted ? "PlayerInfo_Deleted" : "PlayerInfo";

        //从数据库里读取玩家数据
        CDBOperator dbHandle(DATABASE);
        dbHandle->SQL("SELECT AccountId,PlayerName,ActiveDate,lastLogin,lastLogout FROM %s WHERE PlayerId=%d",dbName,playerId);

        if (!dbHandle->More())
            return 0;

        m_playerInfo.BaseData.PlayerId       = playerId;
        m_playerInfo.BaseData.AccountId      = dbHandle->GetInt();
        sStrcpy(m_playerInfo.BaseData.PlayerName,COMMON_STRING_LENGTH,dbHandle->GetString(),COMMON_STRING_LENGTH);
        m_playerInfo.BaseData.createTime     = dbHandle->GetTime();
        m_playerInfo.BaseData.lastLoginTime  = dbHandle->GetTime();
        m_playerInfo.BaseData.lastLogoutTime = dbHandle->GetTime();

        dbHandle->SQL("SELECT DispData FROM %s WHERE PlayerID = %d",dbName,playerId);

        if(dbHandle->Eval())
        {
            int dataSize             = sizeof(stPlayerDisp);
            char *pDataInDB          = (char *)MEMPOOL->Alloc(sizeof(stPlayerDisp));
            int dbDataSize           = sizeof(stPlayerDisp);
            dbHandle->GetBlob(dbDataSize,(PBYTE)pDataInDB);

            int Error = uncompress((Bytef*)&m_playerInfo.DispData,(uLongf*)&dataSize,(const Bytef*)pDataInDB,dbDataSize);

            MEMPOOL->Free((MemPoolEntry)pDataInDB);

            if (Error)
                return 0;
        }

        dbHandle->SQL("SELECT MainData FROM %s WHERE PlayerID = %d",dbName,playerId);

        if(dbHandle->Eval())
        {
            int dataSize             = sizeof(stPlayerMain);
            char *pDataInDB          = (char *)MEMPOOL->Alloc(sizeof(stPlayerMain));
            int dbDataSize           = sizeof(stPlayerMain);
            dbHandle->GetBlob(dbDataSize,(PBYTE)pDataInDB);

            int Error = uncompress((Bytef*)&m_playerInfo.MainData,(uLongf*)&dataSize,(const Bytef*)pDataInDB,dbDataSize);

            MEMPOOL->Free((MemPoolEntry)pDataInDB);

            if (Error)
                return 0;
        }

        return &m_playerInfo;
    }
    else
    {
        PlayerDataRef pPlayer = SERVER->GetPlayerManager()->loadPlayer( playerId);

        if (pPlayer.isNull() || 0 == pPlayer->m_pMainData)
            return 0;

        memcpy(&m_playerInfo.BaseData,&pPlayer->BaseData,  sizeof(m_playerInfo.BaseData));
        memcpy(&m_playerInfo.DispData,&pPlayer->DispData,  sizeof(m_playerInfo.DispData));
        memcpy(&m_playerInfo.MainData,pPlayer->m_pMainData,sizeof(m_playerInfo.MainData));

        int curTime = (int)time(0);

        if (curTime - m_lastGetTime > 60)
        {
            pAccount->RefreshData();
            m_lastGetTime = curTime;
        }
        
        return &m_playerInfo;
    }
}

stPlayerStruct* CGMUser::GetPlayerInfo(const char* playerName,bool isPlayerDeleted)
{
    if (0 == playerName)
        return 0;

    m_isPlayerDeleted = isPlayerDeleted;

    CDBOperator dbHandle(DATABASE);
    dbHandle->SQL("SELECT PlayerId FROM %s WHERE PlayerName='%s'",isPlayerDeleted ? "PlayerInfo_Deleted" : "PlayerInfo",playerName);

    if (!dbHandle->More())
        return false;

    return GetPlayerInfo(dbHandle->GetInt(),isPlayerDeleted);
}