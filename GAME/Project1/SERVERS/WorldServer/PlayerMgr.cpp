
#include <time.h>
#include <assert.h>
#include <sstream>
#include "DBLib/dbLib.h"
#include "WinTcp/dtServerSocket.h"
#include "Common/DumpHelper.h"
#include "zlib/zlib.h"
#include "base/SafeString.h"
#include "Common/MemGuard.h"
#include "PlayerMgr.h"
#include "ChatMgr.h"
#include "TeamManager.h"
#include "WorldServer.h"
#include "ExportScript.h"
#include "Common/WorkQueue.h"
#include "topmanager.h"
#include "CopymapManager.h"

using namespace std;

CPlayerManager::CPlayerManager()
{
	gLock.registerLockable( this, Lock::Player );
}

CPlayerManager::~CPlayerManager()
{

}

int CPlayerManager::ResetPlayerStatus(T_UID UID,int AccountId)
{
	DO_LOCK( Lock::Player );
	
	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

	if(ita == m_AccountMap.end())
		return NONE_ERROR;

	stAccountInfo *paInfo = ita->second;

	if(paInfo->UID != UID)
		return GAME_UID_ERROR;

	paInfo->LastTime	= (int)time(NULL);
	paInfo->CurPlayerPos= -1;
	paInfo->LineId		= 0;

	for(int i=0;i<paInfo->PlayerNum;i++)
	{
		PlayerDataRef pPlayer = loadPlayer( paInfo->PlayerIdList[i]);
		m_PlayerMap.erase(paInfo->PlayerIdList[i]);
		m_PlayerNameMap.erase( pPlayer->BaseData.PlayerName );
		m_LineZonePlayer[paInfo->LineId][pPlayer->DispData.ZoneId].erase(paInfo->PlayerIdList[i]);
	}

	return NONE_ERROR;
}

void CPlayerManager::RemovePlayer( int nPlayerId )
{
	char buf[256];
	sprintf_s( buf, 256, "删除角色数据【%d】", nPlayerId );
	g_Log.WriteLog( buf );
	DO_LOCK( Lock::Player );
	
	PlayerDataRef pPlayerData = GetPlayerData( nPlayerId );
	
	if( !pPlayerData.isNull() )
	{
		int nCopyMapInstId = SERVER->GetCopyMapManager()->GetPlayerCopyMapId( nPlayerId );
		
		// 玩家已离开副本
		if( nCopyMapInstId == 0 )
			pPlayerData->DispData.CopyMapInstId = 1;

		SavePlayerData( pPlayerData );

		int nZoneId = pPlayerData->DispData.ZoneId;
		std::string name = pPlayerData->BaseData.PlayerName;
		stAccountInfo* pinfo = GetAccountInfo( nPlayerId );


		m_PlayerMap.erase(nPlayerId);
		m_PlayerNameMap.erase(name);
		m_LineZonePlayer[pinfo->LineId][nZoneId].erase(nPlayerId);	
	}
}

void CPlayerManager::RemoveAccount( int nAccountId )
{
	char buf[256];
	sprintf_s( buf, 256, "删除帐号数据【%d】", nAccountId );
	g_Log.WriteLog( buf );

	DO_LOCK( Lock::Player );

	HashAccountMap::iterator ita = m_AccountMap.find(nAccountId);

	if(ita == m_AccountMap.end())
		return ;

	stAccountInfo *paInfo = ita->second;

	paInfo->LastTime	= (int)time(NULL);
	paInfo->CurPlayerPos= -1;
	paInfo->LineId		= 0;
	
	for(int i=0;i<paInfo->PlayerNum;i++)
	{
		RemovePlayer( paInfo->PlayerIdList[i] );
	}
}

int CPlayerManager::SavePlayerData( PlayerDataRef pPlayerData )
{
	DO_LOCK( Lock::Player );

	// 加入排行榜统计
	SERVER->GetTopManager()->onNewDataComming( pPlayerData );

	// 更新临时玩家数据
	PlayerDataRef pPlayerDataOld = pPlayerData;

	if( !GetPlayerData( pPlayerData->BaseData.PlayerId ).isNull() )
		return NONE_ERROR;

	return savePlayerData( pPlayerData );
}

int CPlayerManager::SavePlayerData( stPlayerStruct* pPlayerData,bool isFromGMCmd)
{
	DO_LOCK( Lock::Player );

	bool bFound = false;

	if( !pPlayerData )
		return NONE_ERROR;

	if( pPlayerData->BaseData.AccountId == 0 || pPlayerData->BaseData.PlayerId == 0 )
		return NONE_ERROR;

	// 更新临时玩家数据
	PlayerDataRef pPlayerDataOld = GetPlayerData( pPlayerData->BaseData.PlayerId );

	if( pPlayerDataOld.isNull() )
	{
		g_Log.WriteWarn( "玩家不在线，保存离线数据！" );
		pPlayerDataOld = GetOfflinePlayerData( pPlayerData->BaseData.PlayerId );
	}
	// 更新玩家当前的SID
	m_PlayerSIDMap[pPlayerData->BaseData.PlayerId] = pPlayerData->BaseData.SaveUID;

	if( !pPlayerDataOld.isNull() && pPlayerData->BaseData.SaveUID < pPlayerDataOld->BaseData.SaveUID )
	{
		g_Log.WriteError( DumpHelper::DumpStack() );
		std::stringstream ss;
		ss << "过期的玩家数据[" << pPlayerData->BaseData.PlayerId << "]-["<<pPlayerData->BaseData.SaveUID<<"]-["<<pPlayerDataOld->BaseData.SaveUID<<"]";
		g_Log.WriteWarn(ss);
		return NONE_ERROR;
	}
	else
	{

	}

	if( !pPlayerDataOld.isNull() )
	{
		memcpy( &(pPlayerDataOld->BaseData), &(pPlayerData->BaseData), sizeof( stPlayerBase ) );
		memcpy( &(pPlayerDataOld->DispData), &(pPlayerData->DispData), sizeof( stPlayerDisp ) );

		if( pPlayerDataOld->m_pMainData )
		{
			// 更新服务器数据
			pPlayerData->MainData.playerHot = pPlayerDataOld->m_pMainData->playerHot;

			pPlayerData->MainData.SocialItemCount = pPlayerDataOld->m_pMainData->SocialItemCount;

			for( int i = 0; i < pPlayerDataOld->m_pMainData->SocialItemCount; i++ )
			{
				pPlayerData->MainData.SocialItem[i].friendValue = pPlayerDataOld->m_pMainData->SocialItem[i].friendValue;
				pPlayerData->MainData.SocialItem[i].playerId = pPlayerDataOld->m_pMainData->SocialItem[i].playerId;
				pPlayerData->MainData.SocialItem[i].type = pPlayerDataOld->m_pMainData->SocialItem[i].type;
			}
			memcpy( pPlayerDataOld->m_pMainData, &(pPlayerData->MainData), sizeof( stPlayerMain ) );
		}

		bFound = true;
	}

	// 加入排行榜统计
	SERVER->GetTopManager()->onNewDataComming( pPlayerDataOld );


	if( !bFound && !isFromGMCmd)
		return NONE_ERROR;

	return savePlayerData( pPlayerDataOld );
}

int CPlayerManager::VerifyUID(T_UID UID,int AccountId)
{
	DO_LOCK( Lock::Player );

	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

	if(ita == m_AccountMap.end())
	{
        return UNKNOW_ERROR;
    }

    stAccountInfo *pAccountInfo = ita->second;

    if(pAccountInfo->UID > UID)
        return GAME_UID_ERROR;

    return NONE_ERROR;
}

int CPlayerManager::CreatePlayerData(CDBOperator &dbHandle,stPlayerData *pPlayerData)
{
	DO_LOCK( Lock::Player );

	int MaxSize = getMax(sizeof(stPlayerDisp),sizeof(stPlayerMain)) + 128;

	CMemGuard guard(MaxSize);
	char *Buffer = guard.get();

	int Size = MaxSize;
	int error = compress((Bytef *)Buffer,(uLongf*)&Size,(const Bytef*)&pPlayerData->DispData,sizeof(stPlayerDisp));
	if(error){
		return COMPRESS_ERROR;
	}

	dbHandle->SQL("SELECT DispData FROM PlayerInfo WHERE PlayerId=%d",pPlayerData->BaseData.PlayerId);
	if(dbHandle->Eval())
	{
		dbHandle->SetBlob("PlayerInfo","DispData",(PBYTE)Buffer,Size);
	}

	Size = MaxSize;
	error = compress((Bytef *)Buffer,(uLongf*)&Size,(const Bytef*)pPlayerData->m_pMainData,sizeof(stPlayerMain));
	if(error){
		return COMPRESS_ERROR;
	}

	dbHandle->SQL("SELECT MainData FROM PlayerInfo WHERE PlayerId=%d",pPlayerData->BaseData.PlayerId);
	if(dbHandle->Eval())
	{
		dbHandle->SetBlob("PlayerInfo","MainData",(PBYTE)Buffer,Size);
	}

	return NONE_ERROR;
}

int CPlayerManager::CreatePlayer(T_UID UID,int AccountId,const char *PlayerName,int Sex,int Body,int Face,int Hair,int HairCol,U32 ItemId,int ZoneId,int &PlayerId)
{
	DO_LOCK( Lock::Player );
	
	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

	if(ita == m_AccountMap.end())
	{
	    return UNKNOW_ERROR;
	}
		
	stAccountInfo *pAccountInfo = ita->second;

	if(pAccountInfo->UID!=UID)
		return GAME_UID_ERROR;

	if(pAccountInfo->PlayerNum >= MAX_ACTOR_COUNT)
		return PLAYER_FULL;

	int CurPos = pAccountInfo->PlayerNum;
	stPlayerData* pPlayerData;

	MEMPOOL->AllocObj<stPlayerData>(pPlayerData);
	stPlayerMain* mainData;
	MEMPOOL->AllocObj<stPlayerMain>(mainData);
	pPlayerData->m_pMainData = mainData;

	//------------初始化一些必要的数据
	strcpy_s(pPlayerData->BaseData.PlayerName, 32, PlayerName); 
	pPlayerData->BaseData.AccountId	= AccountId; //------------->> 增加： 要在pPlayerData里面保存AccountId; 修正当玩家创建角色并进入游戏时,此属性值为0
	
	strcpy_s(pPlayerData->DispData.LeavePos, 32, "0");		//------> 需该初始化坐标为0，方便策划控制玩家出生点
	pPlayerData->DispData.ZoneId		= ZoneId;
	pPlayerData->DispData.TriggerNo	= 0;
	pPlayerData->DispData.Sex		= (char)Sex;
	pPlayerData->DispData.Body		= (U16)Body;
	pPlayerData->DispData.Face		= (U16)Face;
	pPlayerData->DispData.Hair		= (U16)Hair;
	pPlayerData->DispData.HairCol	= (char)HairCol;
	pPlayerData->DispData.EquipInfo[EQUIPPLACEFLAG_BODY].ItemID = U32(ItemId);
	pPlayerData->DispData.CopyMapInstId = 1; //---------------->> 增加：初始化玩家的副本实例ID
	//-------------------------------

	int Error = DB_ERROR;
	CDBOperator dbHandle(DATABASE);
	try
	{
		dbHandle->SQL("EXECUTE CreatePlayer %d,'%s', %d, %d",AccountId,PlayerName,SERVER->currentAreaId, sizeof( stPlayerStruct ) );		//数据库表写入相关字段并返回	PlayerId，DispData,MainData
		if(dbHandle->More())
		{
			int retCode = dbHandle->GetInt();
			switch(retCode)
			{
			case 0:
				PlayerId = dbHandle->GetInt();
				pPlayerData->BaseData.PlayerId = PlayerId;
				Error = CreatePlayerData(dbHandle,pPlayerData);
				if(Error!=NONE_ERROR)
				{
					dbHandle->SQL("DELETE FROM PlayerInfo WHERE PlayerId=%d",PlayerId);
					dbHandle->Exec();
				}
				break;
			}
		}
	}
	DB_CATCH_LOG(CreatePlayer);

	try
	{
		if(Error != NONE_ERROR)
		{
			dbHandle->SQL("DELETE FROM PlayerInfo WHERE PlayerId=%d",PlayerId);
			dbHandle->Exec();

			MEMPOOL->FreeObj<stPlayerData>(pPlayerData);
			pPlayerData = NULL;
			return Error;
		}
	}
	DB_CATCH_LOG(CreatePlayerFail);

	PlayerDataRef player = pPlayerData;
	savePlayerData( player );

	//添加角色数据
	pAccountInfo->PlayerIdList[CurPos] = pPlayerData->BaseData.PlayerId;
	pAccountInfo->PlayerNum++;

	return NONE_ERROR;
}

int CPlayerManager::DeletePlayer(T_UID UID,int AccountId,int PlayerId)
{
	DO_LOCK( Lock::Player );
	
	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

	if(ita == m_AccountMap.end())
		return UNKNOW_ERROR;

	stAccountInfo *pAccountInfo = ita->second;

	if(pAccountInfo->UID!=UID)
		return GAME_UID_ERROR;

	int CurPos = -1;
	for(int i=0;i<pAccountInfo->PlayerNum;i++)
	{
		if(pAccountInfo->PlayerIdList[i] == PlayerId)
		{
			CurPos = i;
		}
	}

	if(CurPos == -1)
		return UNKNOW_ERROR;

	//清理角色数据
	PlayerDataRef pData = loadPlayer( pAccountInfo->PlayerIdList[CurPos] );

	std::string name = pData->BaseData.PlayerName;
	int zoneId = pData->DispData.ZoneId;

	int Error = DB_ERROR;
	CDBOperator dbHandle(DATABASE);
	try
	{
		dbHandle->SQL("EXECUTE DeletePlayer %d",PlayerId);
		if(dbHandle->More())
		{
			int retCode = dbHandle->GetInt();
			switch(retCode)
			{
			case 0:
				Error = NONE_ERROR;
				break;
			}
		}
	}
	DB_CATCH_LOG(DeletePlayer);

	if(Error!=NONE_ERROR)
	{
		g_Log.WriteError( DumpHelper::DumpStack() );
		return Error;
	}

	//--------------------这里这些虽然只有在角色登录在游戏中才被管理，但为了防止一些没有考虑到的漏洞，还是在这里删除一遍
	m_PlayerMap.erase(PlayerId);
	m_PlayerNameMap.erase(name);
	m_LineZonePlayer[pAccountInfo->LineId][zoneId].erase(PlayerId);
	//----------------------------

	//清理角色管理
	for(int i=CurPos;i<pAccountInfo->PlayerNum-1;i++)
	{
		pAccountInfo->PlayerIdList[i] = pAccountInfo->PlayerIdList[i+1];
	}

	pAccountInfo->PlayerNum--;

    //TODO log
    /*lg_character_delete log;
    log.accountName = pAccountInfo->AccountName;

    if (pPlayer->pPlayerData)
    {
        log.name  = pPlayer->pPlayerData->BaseData.PlayerName;
        log.level = pPlayer->pPlayerData->DispData.Level;
    }
    
    LOG(log);*/

	return NONE_ERROR;
}

bool CPlayerManager::IsGM( int nPlayerId )
{
	DO_LOCK( Lock::Player );

	stAccountInfo* pInfo = GetAccountInfo( nPlayerId );

	if( !pInfo )
		return false;

	return pInfo->isGM;
}

int CPlayerManager::AddPlayerMap(int SocketId,T_UID UID,int LineId,int AccountId,int PlayerId,bool isFirst)
{
	DO_LOCK( Lock::Player );

	stAccountInfo *pAccountInfo = NULL;
	stPlayerData *pPlayerData = NULL;
	int CurPos = -1;

	if(isFirst)
	{
		HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

		if(ita == m_AccountMap.end())
        {
            return UNKNOW_ERROR;
        }

		pAccountInfo = ita->second;

		if(pAccountInfo->UID!=UID)
			return GAME_UID_ERROR;

		int i;
		for( i=0;i<pAccountInfo->PlayerNum;i++)
		{
			if(pAccountInfo->PlayerIdList[i] == PlayerId)
				break;
		}

		if(i==pAccountInfo->PlayerNum)
			return PLAYER_ERROR;

		pAccountInfo->CurPlayerPos = i;
		CurPos = i;
		pAccountInfo->LastTime	= (int)time(NULL);
		pAccountInfo->LineId	= LineId;
		pAccountInfo->socketId	= SocketId;

		m_PlayerMap[PlayerId]   = pAccountInfo;
		pPlayerData = loadPlayer( PlayerId );
		m_LineZonePlayer[pAccountInfo->LineId][pPlayerData->DispData.ZoneId][PlayerId] = pAccountInfo;

		//清除其他没用的角色的主数据
		for(int i=0;i<pAccountInfo->PlayerNum;i++)
		{
			if(pAccountInfo->PlayerIdList[i] == PlayerId )
			{
				CurPos = i;
				m_PlayerNameMap[pPlayerData->BaseData.PlayerName] = pAccountInfo;
				pAccountInfo->CurPlayerPos = CurPos;
				return NONE_ERROR;
			}
		}
	}
	else
	{
		HashAccountMap::iterator itp = m_PlayerMap.find(PlayerId);

		if(itp != m_PlayerMap.end())
		{
			pAccountInfo         =  itp->second;
			pAccountInfo->LineId = LineId;

			if(pAccountInfo->AccountId!=AccountId)
				return PLAYER_ERROR;

			if(pAccountInfo->UID != UID)
				return GAME_UID_ERROR;

			pAccountInfo->LastTime = (int)time(NULL);

			//检查角色的主数据
			for(int i=0;i<pAccountInfo->PlayerNum;i++)
			{
				if(pAccountInfo->PlayerIdList[i] == PlayerId)
				{
					CurPos = i;
					pPlayerData = loadPlayer( PlayerId );
					m_PlayerMap[pPlayerData->BaseData.PlayerId] = pAccountInfo;
					m_PlayerNameMap[pPlayerData->BaseData.PlayerName] = pAccountInfo;

					if(pPlayerData->m_pMainData)
					{
						pAccountInfo->CurPlayerPos = CurPos;
						return NONE_ERROR;
					}
				}
			}
		}
		else
		{
			return PLAYER_ERROR;
		}
	}

	return NONE_ERROR;
}

void CPlayerManager::Update(void)
{
	DO_LOCK( Lock::Player );
    
	int curTime = (int)time(0);

    //移除所有idle状态的account对象
    for (HashAccountMap::iterator iter = m_AccountMap.begin(); iter != m_AccountMap.end();)
    {
        stAccountInfo* pAccountInfo = iter->second;
        
		pAccountInfo->Update(curTime);

        if (pAccountInfo->IsIdle())
        {
			RemoveAccount(pAccountInfo->AccountId);
			deleteAccount(pAccountInfo);
			iter = m_AccountMap.erase( iter );
        }
		else
			iter++;
    }

    //	// 在调试版本中不认为玩家超时
    ////#ifdef _DEBUG
    //	return false;
    ////#endif
    //
    //	//扫描所有的管理对象，超时的被认为是下线删除
    //	int curTime = (int)time(NULL);
    //
    //	HashAccountMap::iterator ita = m_AccountMap.begin();
    //
    //	while(ita!=m_AccountMap.end())
    //	{
    //		stAccountInfo *pAccountInfo = ita->second;
    //		if(curTime - pAccountInfo->LastTime > MAX_DROP_TIME)
    //		{
    //			//清除其他没用的角色数据
    //			//清除其他没用的角色数据
    //			for(int i=0;i<pAccountInfo->PlayerNum;i++)
    //			{
    //				stPlayerData *pPlayerData = pAccountInfo->PlayerStatus[i].pPlayerData;
    //				if(pPlayerData)
    //				{
    //					m_LineZonePlayer[pAccountInfo->LineId][pPlayerData->DispData.ZoneId].erase(pPlayerData->BaseData.PlayerId);
    //
    //					m_PlayerNameMap.erase(pPlayerData->BaseData.PlayerName);
    //					MEMPOOL->FreeObj<stPlayerMain>(pPlayerData->m_pMainData);
    //					MEMPOOL->FreeObj<stPlayerData>(pPlayerData);
    //				}
    //
    //				m_PlayerMap.erase(pAccountInfo->PlayerIdList[i]);
    //			}
    //
    //			MEMPOOL->FreeObj<stAccountInfo>(pAccountInfo);
    //			m_AccountMap.erase(ita++);
    //		}
    //		else
    //			ita++;
    //	}
}

bool CPlayerManager::TimeProcess(bool bExit)
{
    SERVER->GetWorkQueue()->PostEvent(0,0,0,0,WQ_TIMER);
    return true;
}

int CPlayerManager::AddAccount(const stAccountInfo& accountInfo)
{
	DO_LOCK( Lock::Player );
	
	int AccountId = accountInfo.AccountId;

	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

	if(ita != m_AccountMap.end())
	{
		//已经有玩家登录了的话覆盖他
		stAccountInfo *paInfo = ita->second;
		paInfo->UID			  = accountInfo.UID;
		paInfo->LastTime	  = accountInfo.LastTime;
		//paInfo->LineId		  = accountInfo.LineId;
		//paInfo->CurPlayerPos  = -1;
		
		for(int i=0;i<paInfo->PlayerNum;i++)
		{
			PlayerDataRef pData = loadPlayer( paInfo->PlayerIdList[i] );
			m_LineZonePlayer[paInfo->LineId][pData->DispData.ZoneId].erase(paInfo->PlayerIdList[i]);		
		}

		return NONE_ERROR;
	}
    
    //第1次加入时读取玩家数据信息
	stAccountInfo *pNewAccountInfo = NULL;
	MEMPOOL->AllocObj<stAccountInfo>(pNewAccountInfo);
	*pNewAccountInfo = accountInfo;

	//查询所有角色

	CDBOperator dbHandle(DATABASE);
	try
	{
		dbHandle->SQL("SELECT TOP %d PlayerId, dbVersion FROM PlayerInfo WHERE AccountId=%d",MAX_ACTOR_COUNT,AccountId);

		char buf[COMMON_STRING_LENGTH];
		while(dbHandle->More())
		{
			
			int id = dbHandle->GetInt();
			int ver = dbHandle->GetInt();

			if( ver != sizeof( stPlayerStruct) )
			{
				return DB_VERSION_ERROR;
			}

			pNewAccountInfo->PlayerIdList[pNewAccountInfo->PlayerNum] = id;
			pNewAccountInfo->PlayerNum++;
		}

		pNewAccountInfo->LastTime = (int)time(NULL);
		m_AccountMap[AccountId]   = pNewAccountInfo;
	}
	DB_CATCH_LOG(AddAccount);

	return NONE_ERROR;
} 

void CPlayerManager::PackPlayerSelectData(int AccountId,Base::BitStream &SendPacket)
{
	DO_LOCK( Lock::Player );

	char *pDataBuf = NULL;

	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

    if(ita == m_AccountMap.end())
    {
        SendPacket.writeFlag(false);
        return ;
    }

	stAccountInfo *pAccountInfo = ita->second;

	if(pAccountInfo->PlayerNum)
	{
		SendPacket.writeFlag(true);
		int bufSize = MAX_PACKET_SIZE * pAccountInfo->PlayerNum;
		pDataBuf = (char *)MEMPOOL->Alloc(bufSize);
		Base::BitStream pack(pDataBuf,bufSize);
		pack.writeInt(pAccountInfo->PlayerNum,Base::Bit8);
		for(int i=0;i<pAccountInfo->PlayerNum;i++)
		{
			PlayerDataRef pData = loadPlayer( pAccountInfo->PlayerIdList[i] );

			pData->BaseData.WriteData(&pack);
			pData->DispData.WriteData(&pack);
		}

		unsigned long dataSize = pack.getPosition();
		char *pCompressBuf = (char *)MEMPOOL->Alloc(bufSize);
		unsigned long compressSize = bufSize;
		int iret = compress((Bytef *)pCompressBuf,&compressSize,(const Bytef *)pDataBuf,dataSize);
		if (!iret)
		{
			SendPacket.writeInt(compressSize,Base::Bit32);
			SendPacket.writeBits(compressSize*Base::Bit8,pCompressBuf);
		}
		else
			SendPacket.writeInt(0,Base::Bit32);

		MEMPOOL->Free((MemPoolEntry)pDataBuf);
		MEMPOOL->Free((MemPoolEntry)pCompressBuf);
	}
	else
		SendPacket.writeFlag(false);
}

int CPlayerManager::GetSelectedPlayer( int nAccountId )
{
	DO_LOCK( Lock::Player );

	HashAccountMap::iterator it = m_AccountMap.find(nAccountId);
	
	if(it==m_AccountMap.end())
	{
        return 0;
	}

	stAccountInfo* pInfo = it->second;
	
	if( pInfo->CurPlayerPos == -1 )
		return 0;

	return pInfo->PlayerIdList[pInfo->CurPlayerPos];
}

void CPlayerManager::PackPlayerSelectData(int AccountId,int PlayerId,Base::BitStream &SendPacket)
{
	DO_LOCK( Lock::Player );

	char *pDataBuf = NULL;

	HashAccountMap::iterator ita = m_AccountMap.find(AccountId);

    if(ita == m_AccountMap.end())
    {
        SendPacket.writeFlag(false);
        return ;
    }
	
	// 修正重选人物时无法创建新人物的问题(当没有玩家登出消息到WorldServer时) [6/23/2009 LivenHotch]
	{
		stAccountInfo *pAccountInfo = ita->second;

		if(pAccountInfo->PlayerNum>0)
		{
			SendPacket.writeFlag(true);
			int bufSize = MAX_PACKET_SIZE;
			pDataBuf = (char *)MEMPOOL->Alloc(bufSize);
			Base::BitStream pack(pDataBuf,bufSize);
			int CurPos = -1;
			for(int i=0;i<pAccountInfo->PlayerNum;i++)
			{
				if(pAccountInfo->PlayerIdList[i] == PlayerId)
				{
					CurPos = i;
					break;
				}
			}

			if(CurPos != -1)
			{
				pack.writeInt(1,Base::Bit8);

				PlayerDataRef pData = loadPlayer( pAccountInfo->PlayerIdList[CurPos] );
				pData->BaseData.WriteData(&pack);
				pData->DispData.WriteData(&pack);

				unsigned long dataSize = pack.getPosition();
				char *pCompressBuf = (char *)MEMPOOL->Alloc(bufSize);
				unsigned long compressSize = bufSize;
				int iret = compress((Bytef *)pCompressBuf,&compressSize,(const Bytef *)pDataBuf,dataSize);
				if (!iret)
				{
					SendPacket.writeInt(compressSize,Base::Bit32);
					SendPacket.writeBits(compressSize*Base::Bit8,pCompressBuf);
				}
				else
					SendPacket.writeInt(0,Base::Bit32);

				MEMPOOL->Free((MemPoolEntry)pCompressBuf);
			}
			else
			{
				pack.writeInt(0,Base::Bit8);
			}

			MEMPOOL->Free((MemPoolEntry)pDataBuf);
		}
		else
			SendPacket.writeFlag(false);
	}

}

PlayerDataRef CPlayerManager::GetPlayerData(int PlayerId)
{
	///这个方法是没有加锁的
	stPlayerData *pPlayerData = NULL;

	HashAccountMap::iterator itp = m_PlayerMap.find(PlayerId);

	if(itp!=m_PlayerMap.end())
	{
		stAccountInfo *pAccountInfo = itp->second;

		for(int i=0;i<pAccountInfo->PlayerNum;i++)
		{
			if(pAccountInfo->PlayerIdList[i] == PlayerId)
			{
				return loadPlayer(PlayerId);
			}
		}
	}

	return PlayerDataRef();
}

PlayerDataRef CPlayerManager::GetPlayerData( std::string playerName )
{
	return loadPlayer( playerName );
}

void CPlayerManager::SendSimplePacket( int nPlayerId, unsigned short cMessageType, int nDestZoneId, int nSrcZoneId )
{
	DO_LOCK( Lock::Player );

	stAccountInfo* pAccountInfo = GetAccountInfo( nPlayerId );

	if( !pAccountInfo )
	{
		char buf[256];
		sprintf_s( buf, 256, "\n玩家数据为空[%d]", nPlayerId );
		g_Log.WriteError( DumpHelper::DumpStack() + buf );
		return ;
	}

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, cMessageType, pAccountInfo->AccountId, SERVICE_CLIENT, nDestZoneId, nSrcZoneId );
	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
}

void CPlayerManager::SendSimpleInfoPacket( int nPlayerId, unsigned short cMessageType, int nDestZoneId, int nSrcZoneId )
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayerData = GetPlayerData( nDestZoneId );
	stAccountInfo* pAccountInfo = GetAccountInfo( nPlayerId );

	if( pPlayerData.isNull() || !pAccountInfo )
	{
		char buf[256];
		sprintf_s( buf, 256, "\n玩家数据为空[%d]", nPlayerId );
		g_Log.WriteError( DumpHelper::DumpStack() + buf );
		return ;
	}

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, cMessageType, pAccountInfo->AccountId, SERVICE_CLIENT, nDestZoneId, nSrcZoneId );

	pPlayerData->BaseData.WriteData( &SendPacket );
	pPlayerData->DispData.WriteData( &SendPacket );

	SendPacket.writeInt( SERVER->GetTeamManager()->GetTeamId(nDestZoneId), 32 );

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
} 

void CPlayerManager::SendZoneSimplePacket( int nPlayerId,  unsigned short cMessageType, int id, int nSrcZoneId, int param1, int param2 )
{
	DO_LOCK( Lock::Player | Lock::Server );

	stAccountInfo* pAccountInfo = GetAccountInfo( nPlayerId );
	PlayerDataRef pPlayerData = GetPlayerData( nPlayerId );

	if( !pAccountInfo || pPlayerData.isNull() )
	{
		char buf[256];
		sprintf_s( buf, 256, "\n玩家数据为空[%d]", nPlayerId );
		g_Log.WriteError( DumpHelper::DumpStack() + buf );
		return ;
	}

	stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServerByZoneId( pAccountInfo->LineId, pPlayerData->DispData.ZoneId );

	if( !pServerInfo )
	{
		g_Log.WriteError( DumpHelper::DumpStack() + "\n服务器数据为空" );
		return ;
	}

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, cMessageType, id, SERVICE_CLIENT, pPlayerData->DispData.ZoneId, nSrcZoneId );

	SendPacket.writeInt( param1, 32 );
	SendPacket.writeInt( param2, 32 );
	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
}


stAccountInfo *CPlayerManager::GetAccountInfo(int PlayerId)
{
	stAccountInfo* pAccountData = NULL;

	HashAccountMap::iterator itp = m_PlayerMap.find(PlayerId);
	if(itp!=m_PlayerMap.end())
	{
		return itp->second;
	}

	return NULL;
}

stAccountInfo* CPlayerManager::GetAccountMap( int AccountId )
{
	HashAccountMap::iterator it = m_AccountMap.find( AccountId );
	if( it != m_AccountMap.end() )
		return it->second;



	return NULL;
}

int CPlayerManager::PackPlayerData(int PlayerId,Base::BitStream &SendPacket)
{
	DO_LOCK( Lock::Player );

	char *pDataBuf = NULL;
	pDataBuf = (char *)MEMPOOL->Alloc(MAX_PACKET_SIZE);
	Base::BitStream pack(pDataBuf,MAX_PACKET_SIZE);

	PlayerDataRef pPlayerData = GetPlayerData(PlayerId);
	if(pPlayerData.isNull())
	{
		SendPacket.writeInt(0,Base::Bit32);
		MEMPOOL->Free((MemPoolEntry)pDataBuf);
		return 0;
	}

	//写入数据
	pPlayerData->WriteData(&pack);
	pPlayerData->m_pMainData->WriteData(&pack);

	unsigned long dataSize = pack.getPosition();
	unsigned long compressSize = MAX_PACKET_SIZE;
	char *pCompressBuf = (char *)MEMPOOL->Alloc(compressSize);
	int iret = compress((Bytef *)pCompressBuf,&compressSize,(const Bytef *)pDataBuf,dataSize);
	if (!iret)
	{
		SendPacket.writeInt(compressSize,			Base::Bit32);
		SendPacket.writeBits(compressSize*			Base::Bit8,pCompressBuf);
	}
	else
		SendPacket.writeInt(0,						Base::Bit32);

	MEMPOOL->Free((MemPoolEntry)pDataBuf);
	MEMPOOL->Free((MemPoolEntry)pCompressBuf);

	if( pPlayerData->DispData.CopyMapInstId == 1 )
		return pPlayerData->DispData.ZoneId;
	else
		return pPlayerData->DispData.fZoneId;
}

int CPlayerManager::GetPlayerZoneId(int PlayerId)
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayerData = GetPlayerData(PlayerId);

	if(!pPlayerData.isNull())
	{
		if( pPlayerData->DispData.CopyMapInstId == 1 )
			return pPlayerData->DispData.ZoneId;
		else
			return pPlayerData->DispData.fZoneId;
	}

	g_Log.WriteError( DumpHelper::DumpStack() );

	return 0;
}

void CPlayerManager::OnZoneServerRestart(int LineId,int ZoneId)
{
	DO_LOCK( Lock::Player );

	HashLineZonePlayerMap::iterator itl = m_LineZonePlayer.find(LineId);
	if(itl == m_LineZonePlayer.end())
		return;

	HashZonePlayerMap::iterator itz = itl->second.find(ZoneId);
	if(itz == itl->second.end())
		return;

	vector<int> PlayerList;
	HashAccountMap::iterator itp = itz->second.begin();
	while(itp!=itz->second.end())
	{
		PlayerList.push_back(itp->first);
		//这里可以做一些清理工作，因为地图服务器已经宕机后者重启了，一些逻辑可能已经不适合
	}
}

// 获得指定区域内的所有用户
void CPlayerManager::GetZoneAllPlayers( std::list<int>& playerList, int nLineId, int nZoneId )
{

}

// 获得指定区域内的所有用户
void CPlayerManager::GetWorldAllPlayers( std::list<int>& playerList )
{
	DO_LOCK( Lock::Player );

	HashAccountMap::iterator ait;

	for( ait = m_PlayerMap.begin(); ait != m_PlayerMap.end(); ait++ )
	{
		playerList.push_back( ait->second->AccountId );
	}
}

void CPlayerManager::PutPlayersInGate( std::list<int>& playerList, stdext::hash_map<int,stdext::hash_set<int> >& gatePlayerListMap )
{
	DO_LOCK( Lock::Player );
	
	std::list<int>::iterator it;

	stAccountInfo* pAccountInfo;

	for( it = playerList.begin(); it != playerList.end(); it++ )
	{
		pAccountInfo = GetAccountInfo( *it );

		if( pAccountInfo == NULL )
			continue;

		gatePlayerListMap[GetAccountInfo( *it )->socketId].insert( *it );
	}
}

void CPlayerManager::PackPlayerTransDataBase( int nPlayerId, Base::BitStream& packet )
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayer = GetPlayerData( nPlayerId );

	if( pPlayer.isNull() )
		return ;

	// 测试用，写入假的HP和MP
	packet.writeInt( 100, Base::Bit32 );
	packet.writeInt( 100, Base::Bit32 );
}

void CPlayerManager::PackPlayerTransDataNormal( int nPlayerId, Base::BitStream& packet )
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayer = GetPlayerData( nPlayerId );

	if( pPlayer.isNull() )
		return ;

	packet.writeString( pPlayer->BaseData.PlayerName, COMMON_STRING_LENGTH );
}

int CPlayerManager::updatePlayerSID( int playerId )
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayerData = GetPlayerData( playerId );

	if( pPlayerData.isNull() )
	{
		pPlayerData->BaseData.SaveUID++;
		return pPlayerData->BaseData.SaveUID;
	}
	
	g_Log.WriteError( DumpHelper::DumpStack() );

	return NULL;
}

stAccountInfo* CPlayerManager::GetAccountInfoBySocket( int socketId )
{
	int accountId = m_AccountSocketMap[socketId];

	return GetAccountMap( accountId );
}

PlayerDataRef CPlayerManager::GetOfflinePlayerData( int playerId )
{
	return loadPlayer( playerId );
}

PlayerDataRef CPlayerManager::GetOfflinePlayerData( std::string playerName )
{
	return loadPlayer( playerName );
}

PlayerDataRef CPlayerManager::LoadPlayer( int playerId )
{
	return loadPlayer( playerId );
}

PlayerDataRef CPlayerManager::LoadPlayer( std::string playerName )
{
	return loadPlayer( playerName );
}

int CPlayerManager::GetCurrentPlayerSID( int playerId )
{
	DO_LOCK( Lock::Player );

	return m_PlayerSIDMap[playerId];
}

void CPlayerManager::deleteAccount( stAccountInfo* pAccountInfo )
{
	if( !pAccountInfo )
		return ;

	DO_LOCK( Lock::Player );

	MEMPOOL->FreeObj<stAccountInfo>(pAccountInfo);

	return ;
}

CPlayerManager::HashAccountMap& CPlayerManager::GetPlayerMap()
{
	return m_PlayerMap;
}

void CPlayerManager::showAllAccountInfo()
{
	DO_LOCK( Lock::Player );

	printf( "\nPlayerCount: %d\n", m_PlayerMap.size() );
	printf( "|AccountName\t|PlayerName\t|PlayerId\t|Status\t|LineId\t|ZoneId\n" );

	HashAccountMap::iterator it;
	for( it = m_AccountMap.begin(); it != m_AccountMap.end(); it++ )
	{
		stAccountInfo* pInfo = it->second;
		for( int i = 0; i < pInfo->PlayerNum; i++ )
		{
			if( i != pInfo->CurPlayerPos )
				continue;

			PlayerDataRef pPlayerData = loadPlayer( pInfo->PlayerIdList[i] );
			
			int ZoneId = pPlayerData->DispData.ZoneId;
			if( pPlayerData->DispData.CopyMapInstId != 1 )
				ZoneId = pPlayerData->DispData.fZoneId;

			printf( "|%s\t\t|%s\t\t|%d\t\t|%d\t|%d\t|%d\n", pInfo->AccountName, pPlayerData->BaseData.PlayerName, pPlayerData->BaseData.PlayerId, pInfo->getCurrentState(), pInfo->LineId, ZoneId );
		}
	}
}

CmdFunction( ShowAllPlayer )
{
	SERVER->GetPlayerManager()->showAllAccountInfo();
}