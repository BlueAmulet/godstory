#include ".\copymapmanager.h"
#include "Event.h"
#include "TeamManager.h"
#include "ServerMgr.h"
#include "PlayerMgr.h"
#include "WorldServer.h"
#include "zlib/zlib.h"
#include "common/datafile.h"
#include "ExportScript.h"

IMPLEMENT_GAMEDATA_CLASS( CopymapData )
{
	new( getClass(), "副本ID"			)GameDataProperty( GAMEDATA_OFFSET(mId				), GameDataPropertyTypes::TypeU16 );
	new( getClass(), "副本数量上限"		)GameDataProperty( GAMEDATA_OFFSET(mMaxPlayer		), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "副本数量"			)GameDataProperty( GAMEDATA_OFFSET(mMapCount		), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "副本名称"			)GameDataProperty( GAMEDATA_OFFSET(mName			), GameDataPropertyTypes::TypeString );
	new( getClass(), "副本图标"			)GameDataProperty( GAMEDATA_OFFSET(mIcon			), GameDataPropertyTypes::TypeString );
	new( getClass(), "副本文字说明"		)GameDataProperty( GAMEDATA_OFFSET(mDesc			), GameDataPropertyTypes::TypeString );
	new( getClass(), "最高角色等级要求" )GameDataProperty( GAMEDATA_OFFSET(mMaxPlayerLevel	), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "最低角色等级要求" )GameDataProperty( GAMEDATA_OFFSET(mMinPlayerLevel	), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "最大参加人数"		)GameDataProperty( GAMEDATA_OFFSET(mMaxPlayerCount	), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "最小参加人数"		)GameDataProperty( GAMEDATA_OFFSET(mMinPlayerCount	), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "原地复活"			)GameDataProperty( GAMEDATA_OFFSET(mRegen			), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "复活次数"			)GameDataProperty( GAMEDATA_OFFSET(mRegenCount		), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "副本关闭时间"		)GameDataProperty( GAMEDATA_OFFSET(mCloseTime		), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "创建道具ID"		)GameDataProperty( GAMEDATA_OFFSET(mRequireItemId	), GameDataPropertyTypes::TypeU32 );
	new( getClass(), "创建道具数量要求" )GameDataProperty( GAMEDATA_OFFSET(mRequireItemCount), GameDataPropertyTypes::TypeU32 );
	new( getClass(), "创建游戏币"		)GameDataProperty( GAMEDATA_OFFSET(mRequireMoney	), GameDataPropertyTypes::TypeU32 );
	new( getClass(), "加入道具ID"		)GameDataProperty( GAMEDATA_OFFSET(mJoinItemId		), GameDataPropertyTypes::TypeU32 );
	new( getClass(), "加入道具数量要求" )GameDataProperty( GAMEDATA_OFFSET(mJoinItemCount	), GameDataPropertyTypes::TypeU32);
	new( getClass(), "加入游戏币要求"	)GameDataProperty( GAMEDATA_OFFSET(mJoinMoney		), GameDataPropertyTypes::TypeU32 );
	new( getClass(), "中途加入"			)GameDataProperty( GAMEDATA_OFFSET(mJoinable		), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "数据统计"			)GameDataProperty( GAMEDATA_OFFSET(mDataAnaylise	), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "副本评价"			)GameDataProperty( GAMEDATA_OFFSET(mCopymapConsumer	), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "冒险点上限"		)GameDataProperty( GAMEDATA_OFFSET(mMaxAdventureCount ), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "离队计时"			)GameDataProperty( GAMEDATA_OFFSET(mLeaveTime		), GameDataPropertyTypes::TypeU32 );
	new( getClass(), "阵营数量"			)GameDataProperty( GAMEDATA_OFFSET(mPartCount		), GameDataPropertyTypes::TypeU8 );
	new( getClass(), "LOADING图"		)GameDataProperty( GAMEDATA_OFFSET(mLoadingImage	), GameDataPropertyTypes::TypeString );
	new( getClass(), "队长邀请"			)GameDataProperty( GAMEDATA_OFFSET(mLeaderInvation	), GameDataPropertyTypes::TypeU8 );
}

//CopymapData data;
//int m = CopymapData::getProperty("Id")->value<int>( &data );

CCopyMapManager::CCopyMapManager(void)
{
	m_nIdSeed = COPYMAP_IDSEED;
   
    registerEvent(CLIENT_COPYMAP_OpenRequest, &CCopyMapManager::HandleOpenRequest);
	registerEvent(CLIENT_COPYMAP_OpenResponse,&CCopyMapManager::HandleOpenResponse);

    registerEvent(CLIENT_COPYMAP_CloseRequest, &CCopyMapManager::HandleClose);
    registerEvent(CLIENT_COPYMAP_EnterRequest, &CCopyMapManager::HandleEnterRequest);
    registerEvent(CLIENT_COPYMAP_LeaveRequest, &CCopyMapManager::HandleLeave);
	registerEvent(WORLD_COPYMAP_PlayerDataResponse, &CCopyMapManager::HandleCopymapPlayerDataResponse);

	gLock.registerLockable( this, Lock::Copymap );

	//LoadCopyMapInfo();
}

CCopyMapManager::~CCopyMapManager(void)
{
}

void CCopyMapManager::Initialize()
{

}


// [LivenHotch]:  generate a random copy map just for now, it's a fake data
void CCopyMapManager::LoadCopyMapInfo()
{
	CDataFile op;
	RData tempdata;

	if(!op.readDataFile("./Data/CopymapRepository.dat"))
	{
		MessageBoxA( 0, "载入副本DATA错误!","严重错误", MB_OK );
		return;
	}

	CopymapData* pCopymapData;

	for(int i=0; i<op.RecordNum; ++i)
	{
		pCopymapData = new CopymapData();

		for( int j=0; j<op.ColumNum; j++ )
		{
			op.GetData(tempdata);
			if( tempdata.m_Type == GameDataPropertyTypes::TypeString )
			{
				GameDataProperty* pProty = CopymapData::getClass()->getProperty( j );
				if( pProty )
					pProty->value<std::string>( pCopymapData ) = tempdata.m_string;
			}
			else if( tempdata.m_Type == GameDataPropertyTypes::TypeU8 )
			{
				GameDataProperty* pProty = CopymapData::getClass()->getProperty( j );
				if( pProty )
					pProty->value<int>( pCopymapData ) = tempdata.m_U8;
			}
			else
			{
				GameDataProperty* pProty = CopymapData::getClass()->getProperty( j );
				if( pProty )
					pProty->value<int>( pCopymapData ) = tempdata.m_U32;
			}
		}

		stCopyMapTypeInfo typeInfo;
		typeInfo.nId = pCopymapData->mId;
		typeInfo.strName = pCopymapData->mName;

		for( int l=0; l < pCopymapData->mMapCount; l++ )
		{
			stCopyMapTypeMapInfo typeMapInfo;
			typeMapInfo.nMaxCopyMapCount = pCopymapData->mMaxPlayer;
			typeMapInfo.nMaxPlayerCount = pCopymapData->mMaxPlayer * pCopymapData->mMaxPlayerCount;
			typeMapInfo.nMapId = pCopymapData->mId * 100 + ( l + 1 );

			typeInfo.mapMap[typeMapInfo.nMapId] = typeMapInfo;
		}

		m_copyMapInfoTypeMap[typeInfo.nId] = typeInfo;
	}
}


int CCopyMapManager::OpenCopymap( int nPlayerId, int nCopyMapTypeId, int& nCopyMapInstanceId )
{
	DO_LOCK( Lock::Copymap );

	stdext::hash_map<int, stCopyMapTypeInfo>::iterator it;

	it = m_copyMapInfoTypeMap.find( nCopyMapTypeId );
	if( it == m_copyMapInfoTypeMap.end() )
		return COPYMAP_ERROR_NOTEXISTED;

	stCopyMapTypeInfo* pInfo = &(it->second);

	stCopyMapInstInfo cmInst;
	cmInst.nTypeId = pInfo->nId;
	cmInst.nId = m_nIdSeed++;

	int nMapId = FindFreeMap( pInfo->nId );

	if( nMapId == 0 )
		return COPYMAP_ERROR_FULL;

	m_copyMapInfoTypeMap[cmInst.nTypeId].mapMap[cmInst.nMapId].nCopyMapCount++;

	cmInst.nMapId = nMapId;
	cmInst.nCreator = nPlayerId;

	// just for debug for now, must be replaced later
	cmInst.nLineId = 1;

	memcpy( &m_copyMapInfoInstMap[cmInst.nId], &cmInst, sizeof( stCopyMapInstInfo ) );
	nCopyMapInstanceId = cmInst.nId;

	return COPYMAP_ERROR_NONE;
}

int CCopyMapManager::CloseCopyMap( int nId )
{
	DO_LOCK( Lock::Copymap );

	stdext::hash_map<int, stCopyMapInstInfo>::iterator iti;

	iti = m_copyMapInfoInstMap.find( nId );
	if( iti == m_copyMapInfoInstMap.end() )
		return COPYMAP_ERROR_NOTEXISTED;

	//// Only the copy map's creator can close it [2009-5-25 LivenHotch]
	//if( iti->second.nCreator != nPlayerId )
	//	return COPYMAP_ERROR_UNKNOWN;

	// check if the copymap instance have any players
	if( m_copyMapInfoTypeMap[iti->second.nTypeId].mapMap[iti->second.nMapId].nPlayerCount != 0 )
	{
		stdext::hash_map<int,int>::iterator it;
		
		std::list< int > playerList;

		for( it = m_playerCopyMap.begin(); it != m_playerCopyMap.end(); it++ )
		{
			playerList.push_back( it->first );
		}

		for each( int playerId in playerList )
		{
			LeaveCopyMap( playerId );
		}
	}

	m_copyMapInfoTypeMap[iti->second.nTypeId].mapMap[iti->second.nMapId].nCopyMapCount--;

	m_copyMapInfoInstMap.erase( nId );

	return COPYMAP_ERROR_NONE;
}

/*******************************************************************************
	the player enter a copy map
*******************************************************************************/
int CCopyMapManager::EnterCopyMap( int nPlayerId, int nCopyMapInstId )
{
	DO_LOCK( Lock::Copymap | Lock::Player );

	if( m_playerCopyMap[nPlayerId] != 0 )
		return COPYMAP_ERROR_ALREADYIN;

	// add to inst map
	stdext::hash_map<int, stCopyMapInstInfo>::iterator iti;

	iti = m_copyMapInfoInstMap.find( nCopyMapInstId );

	if( iti == m_copyMapInfoInstMap.end() )
		return COPYMAP_ERROR_NOTEXISTED;

	stCopyMapTypeMapInfo& info = m_copyMapInfoTypeMap[iti->second.nTypeId].mapMap[iti->second.nMapId];
	if( info.nPlayerCount == info.nMaxPlayerCount )
		return COPYMAP_ERROR_FULL;

	info.nCopyMapCount++;

	PlayerDataRef playerData = SERVER->GetPlayerManager()->loadPlayer( nPlayerId );

	if( !playerData.isNull() )
		playerData->DispData.fZoneId = info.nMapId;

	// add to player map
	m_playerCopyMap[nPlayerId] = nCopyMapInstId;
	m_copyMapPlayer[nCopyMapInstId].insert( nPlayerId );

	return COPYMAP_ERROR_NONE;
}

// [LivenHotch]: NOTICE:!!!
// LeaveCopyMap must be called before CloseCopyMap
int CCopyMapManager::LeaveCopyMap( int nPlayerId )
{
	DO_LOCK( Lock::Copymap );

	if( m_playerCopyMap.find(nPlayerId) == m_playerCopyMap.end() )
		return COPYMAP_ERROR_NOTEXISTED;

	int nInstId = m_playerCopyMap[nPlayerId];

	stCopyMapInstInfo& instInfo = m_copyMapInfoInstMap[nInstId];
	m_copyMapInfoTypeMap[instInfo.nTypeId].mapMap[instInfo.nMapId].nPlayerCount--;

	m_playerCopyMap.erase( nPlayerId );
	m_copyMapPlayer[nInstId].erase( nPlayerId );


	if( m_copyMapPlayer[nInstId].size() == 0 )
		m_copyMapPlayer.erase( nInstId );

	return COPYMAP_ERROR_NONE;
}

// [LivenHotch]: find a map server to supprot a new copy map
int CCopyMapManager::FindFreeMap( int nCopyMapTypeId )
{
	// inner call, no need to lock
	stdext::hash_map<int, stCopyMapTypeInfo>::iterator it;

	it = m_copyMapInfoTypeMap.find( nCopyMapTypeId );
	if( it == m_copyMapInfoTypeMap.end() )
		return 0;

	stCopyMapTypeInfo& info = it->second;

	stdext::hash_map<int, stCopyMapTypeMapInfo>::iterator itt;

	for( itt = info.mapMap.begin(); itt != info.mapMap.end(); itt++ )
	{
		if( itt->second.nCopyMapCount < itt->second.nMaxCopyMapCount )
			return itt->second.nMapId;
	}

	return 0;
}

// handle open copy map request
void CCopyMapManager::HandleOpenRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;
	int nCopyMapTypeId = pHead->DestZoneId;
	int nCopyMapInstId = 0;

	int srcZoneId = pHead->SrcZoneId;

	DO_LOCK( Lock::Player | Lock::Server );

	int srcLineId = SERVER->GetServerManager()->GetLineId( SocketHandle );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );

	if( pPlayer.isNull() || !pAccountInfo )
		return ;

	int nErr = OpenCopymap( nPlayerId, nCopyMapTypeId, nCopyMapInstId );

	if( nErr == COPYMAP_ERROR_NONE )
		nErr = EnterCopyMap( nPlayerId, nCopyMapInstId );

	int nZoneId = m_copyMapInfoInstMap[nCopyMapInstId].nMapId;
	int nLineId = pAccountInfo->LineId;

	if( !NotifyZoneOpenCopyMap( nLineId, nZoneId, nCopyMapInstId, srcZoneId, srcLineId, nPlayerId ) )
	{
		// 发送不成功，说明创建或进入失败，返回响应

		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_OpenResponse, nCopyMapInstId, SERVICE_CLIENT, pHead->SrcZoneId, nPlayerId, nErr );
		pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );


		SERVER->GetServerSocket()->Send( SocketHandle, SendPacket );
	}
	//CMemGuard buf( MAX_PACKET_SIZE );
	//Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	//stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_OpenResponse, pAccountInfo->AccountId, SERVICE_CLIENT, nCopyMapInstId, nErr );
	//pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );


	//SERVER->GetServerSocket()->Send( SocketHandle, SendPacket );
}

// handle close copy map request
// we do not send any response
void CCopyMapManager::HandleClose( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nCopyMapInstId = pHead->Id;
	int nZoneId = pHead->SrcZoneId;

	CloseCopyMap( nCopyMapInstId );

	// the zone server will kick all players in this copy map
	NotifyZoneCloseCopyMap( SocketHandle, nZoneId, nCopyMapInstId );
}

bool CCopyMapManager::SendCopymapPlayerData( int PlayerId, int ZoneId, int copymapInstId )
{
	CMemGuard Buffer(MAX_PACKET_SIZE);
	Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
	stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,WORLD_COPYMAP_PlayerData,PlayerId,SERVICE_ZONESERVER,ZoneId, copymapInstId);
	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( PlayerId );

	if( !pAccountInfo )
		return false;
	
	SendPacket.writeInt(pAccountInfo->UID,UID_BITS);
	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( PlayerId );
	
	if( !pPlayerData.isNull() )
	{
		pPlayerData->DispData.fZoneId = ZoneId;
		pPlayerData->DispData.CopyMapInstId = copymapInstId;
	}

	SERVER->GetPlayerManager()->PackPlayerData(PlayerId,SendPacket);

	// 传送其他数据
	stTeamInfo teamInfo;
	SERVER->GetTeamManager()->GetPlayerTeamInfo( PlayerId, teamInfo );
	teamInfo.WritePacket( &SendPacket );

	pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
	int SocketId = SERVER->GetServerManager()->GetZoneSocket( 1, ZoneId );

	if(SocketId)
		SERVER->GetServerSocket()->Send(SocketId,SendPacket);
	else
		return false;

	return true;
}

// handle enter copy map request
void CCopyMapManager::HandleEnterRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;
	int value = pHead->DestZoneId;

	int nCopyMapInstId;

	if( value > 9999 )
		nCopyMapInstId = value;

	DO_LOCK( Lock::Player | Lock::Team );

	//处理地图切换
	T_UID UID		= pPacket->readInt(UID_BITS);
	int LineId		= pPacket->readInt(16);
	int TriggerId	= pPacket->readInt(32);
	int ZoneId	    = pPacket->readInt(16);
	int x		    = pPacket->readSignedInt(32);
	int y		    = pPacket->readSignedInt(32);
	int z		    = pPacket->readSignedInt(32);

	int ZipBufSize	= pPacket->readInt(16);
	U8 *pZipBuf		= MEMPOOL->Alloc(ZipBufSize);
	pPacket->readBits(ZipBufSize*Base::Bit8,pZipBuf);

	int Size		= MAX_PACKET_SIZE;
	U8 *pUnZipBuf	= MEMPOOL->Alloc(Size);
	int iret        = uncompress((Bytef*)pUnZipBuf,(uLongf*)&Size,(const Bytef *)pZipBuf,ZipBufSize);

	if (!iret)
	{
		Base::BitStream dataPacket(pUnZipBuf,Size);
		stPlayerStruct PlayerObject;
		PlayerObject.ReadData(&dataPacket);

		SERVER->GetPlayerManager()->SavePlayerData( &PlayerObject ); // 保存由ZoneServer 发来的数据
	}

	MEMPOOL->Free(pZipBuf);
	MEMPOOL->Free(pUnZipBuf);

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( !pAccountInfo )
		 return ;

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );

	if( pPlayer.isNull() )
		return ;

	int nErr = COPYMAP_ERROR_NONE;

	if( value < 9999 )
	{
		nCopyMapInstId = m_playerCopyMap[nPlayerId];

		// 玩家之前没有进入过任何副本
		if( nCopyMapInstId == 0 )
		{
			// 搜索玩家队友所进入的副本
			CTeam* pTeam = SERVER->GetTeamManager()->GetTeamByPlayer( nPlayerId );

			if( pTeam )
			{
				std::list<int>* pTeammate = pTeam->GetTeammate();

				for each( int tId in *pTeammate )
				{
					PlayerDataRef pTPlayer = SERVER->GetPlayerManager()->GetPlayerData( tId );
					if( pTPlayer.isNull() )
						continue;

					if( pTPlayer->DispData.fZoneId / 100 == value )
					{
						nCopyMapInstId = pTPlayer->DispData.CopyMapInstId;
						break;
					}
				}
			}

			// 玩家的队友也没有进入过此类型副本
			if( nCopyMapInstId == 0 )
			{
				nErr = OpenCopymap( nPlayerId, value, nCopyMapInstId );
	
				if( nErr == COPYMAP_ERROR_NONE )
					nErr = EnterCopyMap( nPlayerId, nCopyMapInstId );
			}
			else
			{
				nErr = EnterCopyMap( nPlayerId, nCopyMapInstId );
			}
			
		}
	}
	else
		nErr = EnterCopyMap( nPlayerId, nCopyMapInstId );

	if( nErr != COPYMAP_ERROR_NONE )
	{
		SendClientEnterResponse( nErr, nPlayerId, 0 );
	}
	else
	{
		bool bRet = SendCopymapPlayerData( nPlayerId, m_copyMapInfoInstMap[nCopyMapInstId].nMapId, nCopyMapInstId );

		if( !bRet )
		{
			LeaveCopyMap( nPlayerId );
			SendClientEnterResponse( COPYMAP_ERROR_NOSERVER, nPlayerId, 0 );
		}
	}
}

// 处理队员传送至副本
void CCopyMapManager::HandleEnterRequest2( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;
	int nCopyMapId = pHead->DestZoneId;

	DO_LOCK( Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( !pAccountInfo )
		return ;

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );

	if( pPlayer.isNull() )
		return ;

	//处理地图切换
	T_UID UID		= pPacket->readInt(UID_BITS);
	int LineId		= pPacket->readInt(16);
	int TriggerId	= pPacket->readInt(32);
	int ZoneId	    = pPacket->readInt(16);
	int x		    = pPacket->readSignedInt(32);
	int y		    = pPacket->readSignedInt(32);
	int z		    = pPacket->readSignedInt(32);

	int ZipBufSize	= pPacket->readInt(16);
	U8 *pZipBuf		= MEMPOOL->Alloc(ZipBufSize);
	pPacket->readBits(ZipBufSize*Base::Bit8,pZipBuf);

	int Size		= MAX_PACKET_SIZE;
	U8 *pUnZipBuf	= MEMPOOL->Alloc(Size);
	int iret        = uncompress((Bytef*)pUnZipBuf,(uLongf*)&Size,(const Bytef *)pZipBuf,ZipBufSize);

	if (!iret)
	{
		Base::BitStream dataPacket(pUnZipBuf,Size);
		stPlayerStruct PlayerObject;
		PlayerObject.ReadData(&dataPacket);

		SERVER->GetPlayerManager()->SavePlayerData( &PlayerObject ); // 保存由ZoneServer 发来的数据
	}

	MEMPOOL->Free(pZipBuf);
	MEMPOOL->Free(pUnZipBuf);

	int nErr = COPYMAP_ERROR_NONE;

	CTeam* pTeam = SERVER->GetTeamManager()->GetTeamByPlayer( nPlayerId );
	if( !pTeam )
	{
		nErr = COPYMAP_ERROR_UNKNOWN;
		SendClientEnterResponse( nErr, nPlayerId, 0 );
		return;
	}

	std::list<int>* pTeammate = pTeam->GetTeammate();

	int nCopyMapInstId = 0;

	for each( int tId in *pTeammate )
	{
		PlayerDataRef pTPlayer = SERVER->GetPlayerManager()->GetPlayerData( tId );
		if( pTPlayer.isNull() )
			continue;

		if( pTPlayer->DispData.fZoneId == nCopyMapId )
		{
			nCopyMapInstId = pTPlayer->DispData.CopyMapInstId;
			break;
		}
	}

	if( nCopyMapInstId == 0 )
	{
		nErr = COPYMAP_ERROR_NOTEXISTED;
		SendClientEnterResponse( nErr, nPlayerId, 0 );
		return;
	}

	nErr = EnterCopyMap( nPlayerId, nCopyMapInstId );

	if( nErr != COPYMAP_ERROR_NONE )
	{
		SendClientEnterResponse( nErr, nPlayerId, 0 );
	}
	else
	{
		bool bRet = SendCopymapPlayerData( nPlayerId, m_copyMapInfoInstMap[nCopyMapInstId].nMapId, nCopyMapInstId );

		if( !bRet )
		{
			LeaveCopyMap( nPlayerId );
			SendClientEnterResponse( COPYMAP_ERROR_NOSERVER, nPlayerId, 0 );
		}
	}
}

void CCopyMapManager::SendClientEnterResponse( int nErr, int playerId, int nCopyMapInstId )
{
	DO_LOCK( Lock::Server | Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( playerId );
	if( !pAccountInfo )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_EnterResponse, pAccountInfo->AccountId, SERVICE_CLIENT, nCopyMapInstId, nErr  );

	// get the copy map info
	stCopyMapInstInfo& instInfo = m_copyMapInfoInstMap[nCopyMapInstId];
	int nIp;
	int nPort;
	SERVER->GetServerManager()->GetZoneIpPort( pAccountInfo->LineId, instInfo.nMapId, nIp, nPort );
	SendPacket.writeInt( nIp, 32 );
	SendPacket.writeInt( nPort, 16 );

	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
}

// handle leave copy map request
// we do not send any response
void CCopyMapManager::HandleLeave( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;
	int nCopyMapInstId = m_playerCopyMap[nPlayerId];

	DO_LOCK( Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( !pAccountInfo )
		return ;

	LeaveCopyMap( nPlayerId );

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_LeaveResponse, nPlayerId, SERVICE_ZONESERVER, pHead->SrcZoneId, pAccountInfo->LineId );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, SendPacket );
}

/************************************************************************/
/* 
	the following two methods may cause a problem        

	When the client receive the open copy map early than the zone server
	and the client send enter copy map request immediately, the zone server
	haven't been ready for the new copy map, then the client would fail to enter
	the copy map.
*/
/************************************************************************/

bool CCopyMapManager::NotifyZoneOpenCopyMap( int nLineId, int nZoneId, int nCopyMapInstId, int nSrcZoneId, int nSrcLineId, int nPlayerId )
{
	stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServerByZoneId( nLineId, nZoneId );

	if( !pServerInfo )
		return false;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_OpenRequest, nCopyMapInstId, SERVICE_ZONESERVER, nZoneId, nPlayerId );
	SendPacket.writeInt( nSrcZoneId, 32 );
	SendPacket.writeInt( nSrcLineId, 32 );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pServerInfo->SocketId, SendPacket );
	return true;
}

void CCopyMapManager::NotifyZoneCloseCopyMap( int socketHandle, int nZoneId, int nCopyMapInstId )
{
	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_COPYMAP_CloseRequest, nCopyMapInstId, SERVICE_ZONESERVER, nZoneId );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( socketHandle, SendPacket );
}

void CCopyMapManager::HandleCopymapPlayerDataResponse( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int playerId = pHead->Id;

	int copymapInstId = pHead->DestZoneId;

	this->SendClientEnterResponse( COPYMAP_ERROR_NONE, playerId, copymapInstId );
}

int CCopyMapManager::GetPlayerCopyMapId( int nPlayerId )
{
	DO_LOCK( Lock::Copymap );

	return m_playerCopyMap[nPlayerId];
}

// 来自Zone的副本开启响应
void CCopyMapManager::HandleOpenResponse( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nLineId = pPacket->readInt( 32 );
	int nZoneId = pPacket->readInt( 32 );
	int nPlayerId = pPacket->readInt( 32 );
	int nCopyInstId = pHead->Id;

	DO_LOCK( Lock::Copymap | Lock::Server );

	int SocketId = SERVER->GetServerManager()->GetZoneSocket( nLineId, nZoneId );
	if( SocketId != 0 )
	{
		CMemGuard buf( MAX_PACKET_SIZE );
		Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
		stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_COPYMAP_OpenResponse, nCopyInstId, SERVICE_ZONESERVER, nZoneId, nPlayerId );

		pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

		SERVER->GetServerSocket()->Send( SocketId, sendPacket );
	}
}

void CCopyMapManager::ShowStatus()
{

	stdext::hash_map<int,stCopyMapInstInfo>::iterator it;
	for( it = m_copyMapInfoInstMap.begin(); it != m_copyMapInfoInstMap.end(); it ++ )
	{
		int nType = it->second.nTypeId;
		printf( "Copymap Type: %d\n", nType );

		stdext::hash_map<int,stCopyMapTypeInfo>::iterator itl = m_copyMapInfoTypeMap.find( nType );

		if( itl == m_copyMapInfoTypeMap.end() )
			continue;

		stCopyMapTypeInfo& info = itl->second;

		stdext::hash_map<int, stCopyMapTypeMapInfo>::iterator itt;

		for( itt = info.mapMap.begin(); itt != info.mapMap.end(); itt++ )
		{
			int playerCount = itt->second.nPlayerCount;
			int copyMapCount = itt->second.nCopyMapCount;

			printf("PlayerCount: %d\t\tCopymapCount: %d\n", playerCount, copyMapCount );
		}
	}
}

CmdFunction( ShowCopymap )
{
	SERVER->GetCopyMapManager()->ShowStatus();
}



CGroup::CGroup()
{

}

CGroup::~CGroup()
{

}

CGroup::Iterator CGroup::begin()
{
	return playerList.begin();
}

CGroup::Iterator CGroup::end()
{
	return playerList.end();
}

CGroup::Iterator CGroup::erase( Iterator& where )
{
	return playerList.erase( where );
}

int CGroup::size()
{
	return (int)playerList.size();
}

void CGroup::addPlayer( int playerId )
{
	playerList.push_back( playerId );
}