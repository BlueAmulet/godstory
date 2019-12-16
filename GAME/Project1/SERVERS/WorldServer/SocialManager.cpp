#include "Common/CommonServerClient.h"
#include "SocialManager.h"
#include "PlayerMgr.h"
#include "WorldServer.h"
#include "ServerMgr.h"
#include "common/omputils.h"
#include "zlib/zlib.h"

int CSocialManager::makeLink( int playerId1, int playerId2, SocialType::Type type )
{
	if( playerId1 ==  playerId2 )
		return SocialError::Self;

	DO_LOCK( Lock::Player | Lock::Social );

	if( mNoAddedMap[playerId2] )
		return SocialError::AutoRefused;

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( playerId1 );

	if( pPlayerData.isNull() )
		return SocialError::Unknown;

	int nCount = 0;
	int firstPlayer = 0;
	int isFind = 0;

	for( int i = 0; i < pPlayerData->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayerData->m_pMainData->SocialItem[i].playerId == playerId2 )
		{
			isFind = 1;
			break;
		}
	}

	if( isFind )
		return SocialError::Existed;

	for( int i = 0; i < pPlayerData->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayerData->m_pMainData->SocialItem[i].type == type )
		{
			firstPlayer = pPlayerData->m_pMainData->SocialItem[i].playerId;
			nCount++;
		}
	}
	
	// type is full, send error
	if( nCount > SocialTypeMaxCount[type] )
	{
		if( type != SocialType::Enemy && type != SocialType::Temp )
		{
			return SocialError::MaxCount;
		}
		else
		{
			// 当仇人或临时好友添加满时，删除掉最早的那个仇人或临时好友
			if( firstPlayer != 0 )
				destoryLink( playerId1, firstPlayer );
		}
	}

	int index = pPlayerData->m_pMainData->SocialItemCount++;
	pPlayerData->m_pMainData->SocialItem[ index ].playerId = playerId2;
	pPlayerData->m_pMainData->SocialItem[ index ].type = type;

	PlayerDataRef pAnother = SERVER->GetPlayerManager()->GetPlayerData( playerId2 );
	if( !pAnother.isNull() && pAnother->m_pMainData )
	{
		bool isFound = false;
		for( int i = 0; i < pAnother->m_pMainData->SocialItemCount; i++ )
		{
			if( pAnother->m_pMainData->SocialItem[i].playerId == playerId1 )
			{
				int type = pAnother->m_pMainData->SocialItem[i].type;
				if( type != SocialType::Enemy && type != SocialType::Temp )
				{
					isFound = true;
					break;
				}
			}
		}

		// 如果双方互为好友，增加一点好友度
		if( isFound )
		{
			addFirendValue( playerId1, playerId2, 1 );
		}
	}

	mLinkManMap[ playerId2 ][playerId1] = type;

	notifyZonePlayerSocialInfo( playerId1 );

	return 0;
}

int CSocialManager::destoryLink( int playerId1, int playerId2, bool isClientDestory )
{
	if( playerId1 ==  playerId2 )
		return SocialError::Self;

	DO_LOCK( Lock::Player | Lock::Social );

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( playerId1 );

	bool isFound = false;

	mLinkManMap[ playerId2 ].erase( playerId1 );

	for( int i = 0; i < pPlayerData->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayerData->m_pMainData->SocialItem[i].playerId == playerId2  && ( isClientDestory ? pPlayerData->m_pMainData->SocialItem[i].type == SocialType::Friend : true ) )
		{
			isFound = true;
			for( int j = i; j < pPlayerData->m_pMainData->SocialItemCount - 1; j++ )
			{
				pPlayerData->m_pMainData->SocialItem[j].playerId = pPlayerData->m_pMainData->SocialItem[j+1].playerId;
				pPlayerData->m_pMainData->SocialItem[j].type = pPlayerData->m_pMainData->SocialItem[j+1].type;
			}

			pPlayerData->m_pMainData->SocialItemCount--;
			break;
		}
	}

	notifyZonePlayerSocialInfo( playerId1 );

	if( isFound )
		return 0;

	return -1;

}

CSocialManager::CSocialManager()
{
	registerEvent( CLIENT_WORLD_QueryFriendRequest,	&CSocialManager::HandleClientWorldFindFriendRequest	);
	registerEvent( CLIENT_WORLD_MakeLinkRequest,	&CSocialManager::HandleClientWorldMakeRequest		);
	registerEvent( CLIENT_WORLD_DestoryLinkRequest, &CSocialManager::HandleClientWorldDestoryRequest	);
	registerEvent( CLIENT_WORLD_ChearRequest,		&CSocialManager::HandleClientWorldChearRequest		);
	registerEvent( CLIENT_WORLD_FindPlayerRequest,	&CSocialManager::HandleClientWorldFindPlayerRequest );
	registerEvent( CLIENT_WORLD_SocialInfoRequest,	&CSocialManager::HandleClientWorldSocialInfoRequest	);
	registerEvent( ZONE_WORLD_MakeLinkRequest,		&CSocialManager::HandleZoneWorldMakeRequest			);
	registerEvent( ZONE_WORLD_DestoryLinkRequest,	&CSocialManager::HandleZoneWorldDestoryRequest		);
	registerEvent( ZONE_WORLD_ChangeLinkRequest,	&CSocialManager::HandleZoneWorldChangeLinkReuquest	);
	registerEvent( ZONE_WORLD_AddFriendValue,		&CSocialManager::HandleZoneWorldAddFriendValue		);
	registerEvent( ZONE_WORLD_PlayerStatusChangeRequest, &CSocialManager::HandleZoneWorldPlayerStatusChangeRequest);
	registerEvent( WORLD_CLIENT_SocialNoAddedRequest, &CSocialManager::HandleClientWorldNoAdded);
	registerEvent( CLIENT_WORLD_QueryPlayerHotRequest, &CSocialManager::HandleQueryPlayerHot );
	registerEvent( CLIENT_WORLD_QueryChearCountRequest, &CSocialManager::HandleQueryChearCount );
	registerEvent( ZONE_WORLD_EncourageNotify, &CSocialManager::HandleZoneWorldEncourageNotify );
	registerEvent( ZONE_WORLD_LotRequest, &CSocialManager::HandleZoneWorldLotRequest );
	registerEvent( ZONE_WORLD_AddToLotRequest, &CSocialManager::HandleZoneWorldAddLotRequest );
	registerEvent( ZONE_WORLD_RemoveFromLotRequest, &CSocialManager::HandleZoneWorldRemoveLotRequest );
	registerEvent( CLIENT_WORLD_PlayerInfoRequest, &CSocialManager::HandleClientWorldPlayerInfoRequest );
	
	gLock.registerLockable( this, Lock::Social );
}

CSocialManager::~CSocialManager()
{

}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldMakeRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );

	SocialType::Type type;

	if( pHead->SrcZoneId == 0 )
		type = SocialType::Friend;
	else
		type = SocialType::Temp;

	int nError = makeLink( pHead->Id, pHead->DestZoneId, type );

	SERVER->GetPlayerManager()->SendSimplePacket( pHead->Id, CLIENT_WORLD_MakeLinkResponse, nError, pHead->DestZoneId );

	if( nError == SocialError::None )
	{
		SERVER->GetPlayerManager()->SendSimpleInfoPacket( pHead->DestZoneId, WORLD_CLIENT_SocialAdded, pHead->Id, pHead->SrcZoneId );
	}
}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldDestoryRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );

	int nError = destoryLink( pHead->Id, pHead->DestZoneId, true );

	SERVER->GetPlayerManager()->SendSimplePacket( pHead->Id, CLIENT_WORLD_DestoryLinkResponse, nError, pHead->DestZoneId );
}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldPlayerInfoRequest )
{
	DO_LOCK( Lock::Player );

	int error = NONE_ERROR;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_PlayerInfoResponse, pHead->Id, SERVICE_CLIENT );

	PlayerDataRef pData = SERVER->GetPlayerManager()->GetOfflinePlayerData( pHead->DestZoneId );

	if( pData.isNull() || !pData->m_pMainData )
		error = PLAYER_ERROR;

	CMemGuard dataBuf( MAX_PACKET_SIZE );
	Base::BitStream dataStream( dataBuf.get(), MAX_PACKET_SIZE );
	if( error == NONE_ERROR )
	{
		pData->WriteData( &dataStream );
		pData->m_pMainData->WriteData( &dataStream );
	}

	CMemGuard zipBuf( MAX_PACKET_SIZE );
	int size = MAX_PACKET_SIZE;
	
	compress( (Bytef*)zipBuf.get(), (uLongf*)&size, (Bytef*)dataBuf.get(), dataStream.getPosition() );
	if( size == 0 )
		error = COMPRESS_ERROR;

	sendPacket.writeFlag( error == NONE_ERROR );

	if( size >= 0 )
	{
		sendPacket.writeInt( size, 20 );
		sendPacket.writeBits( size * 8, zipBuf.get() );
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}

void CSocialManager::EVENT_FUNCTION( HandleQueryPlayerHot )
{
	DO_LOCK( Lock::Player | Lock::Social );

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( pHead->Id );

	if( pPlayerData.isNull() )
		return ;

	SERVER->GetPlayerManager()->SendSimplePacket( pPlayerData->BaseData.PlayerId, CLIENT_WORLD_QueryPlayerHotResponse, pPlayerData->m_pMainData->playerHot );
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldAddLotRequest )
{
	DO_LOCK( Lock::Social );

	int nPlayerId = pHead->Id;

	int sex = pHead->DestZoneId;

	if( sex == 1 )
	{
		mMaleList.push_back( nPlayerId );
	}
	else
	{
		mMaleList.push_back( nPlayerId );
	}
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldRemoveLotRequest )
{
	DO_LOCK( Lock::Social );

	int nPlayerId = pHead->Id;

	int sex = pHead->DestZoneId;

	if( sex == 1 )
	{
		PlayerList::iterator it;
		for( it = mMaleList.begin(); it != mMaleList.end(); it++ )
		{
			if( *it == nPlayerId )
			{
				mMaleList.erase( it );
				return ;
			}
		}
	}
	else
	{
		PlayerList::iterator it;
		for( it = mFemaleList.begin(); it != mFemaleList.end(); it++ )
		{
			if( *it == nPlayerId )
			{
				mFemaleList.erase( it );
				return ;
			}
		}
	}
}

// 处理命运有缘人请求
void CSocialManager::EVENT_FUNCTION( HandleZoneWorldLotRequest )
{
	// 当玩家到某个等级的时候，寻找命运有缘人
	int nPlayerId = pHead->Id;

	DO_LOCK( Lock::Player | Lock::Server | Lock::Social );

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );
	if( pPlayer.isNull() )
		return ;

	PlayerList* playerList = NULL;

	if( pPlayer->DispData.Sex == 1 ) // 男滴
	{
		playerList = &mFemaleList;
	}
	else // 女滴
	{
		playerList = &mMaleList;
	}

	if( playerList == NULL )
		return ;

	int nDestPlayerId = 0;

	if( playerList->size() > 0 )
		nDestPlayerId = *playerList->begin();

	if( nDestPlayerId == 0 )
		return ;

	playerList->erase( playerList->begin() );

	// 把两个人互相加为好友
	makeLink( nPlayerId, nDestPlayerId, SocialType::Friend );
	makeLink( nDestPlayerId, nPlayerId, SocialType::Friend );

	PlayerDataRef pDestPlayer = SERVER->GetPlayerManager()->GetPlayerData( nDestPlayerId );
	if( pDestPlayer.isNull() )
		return ;

	PlayerDataRef pSrcPlayer = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );
	if( pSrcPlayer.isNull() )
		return ;

	stAccountInfo* pInfo = SERVER->GetPlayerManager()->GetAccountInfo( nDestPlayerId );
	if( !pInfo )
		return ;

	stAccountInfo* pInfo2 = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( !pInfo2 )
		return ;

	int nZoneId = pDestPlayer->DispData.ZoneId;
	if( pDestPlayer->DispData.CopyMapInstId != 1 )
		nZoneId = pDestPlayer->DispData.fZoneId;

	int nZoneId2 = pSrcPlayer->DispData.ZoneId;
	if( pSrcPlayer->DispData.CopyMapInstId != 1 )
		nZoneId2 = pSrcPlayer->DispData.fZoneId;

	int socketId = SERVER->GetServerManager()->GetZoneSocket( pInfo->LineId, nZoneId );

	if( socketId == 0 )
		return ;

	// 通知低等级玩家
	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, ZONE_WORLD_LotResponse, nDestPlayerId, SERVICE_ZONESERVER, nZoneId, nPlayerId  );

	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( socketId, sendPakcet );

	socketId = SERVER->GetServerManager()->GetZoneSocket( pInfo2->LineId, nZoneId2 );

	// 通知高等级玩家
	Base::BitStream sendPacket2( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead2 =  IPacket::BuildPacketHead( sendPacket2, ZONE_WORLD_LotNotify, nPlayerId, SERVICE_ZONESERVER, nZoneId2, nDestPlayerId );

	pSendHead2->PacketSize = sendPacket2.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( socketId, sendPacket2 );
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldEncourageNotify )
{
	DO_LOCK( Lock::Player | Lock::Social );

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( pHead->Id );
	
	if( pPlayer.isNull() )
		return ;

	stAccountInfo* pInfo;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, WORLD_CLIENT_EncourageNotify, 0, SERVICE_CLIENT );

	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	int playerCount = pPlayer->m_pMainData->SocialItemCount;
	int i;

	// OpenMP 优化
#pragma OMP_PARALLEL OMP_PRIVATE( i, pInfo )
{
	OMP_CHECK_THREADS();

	for( int i = 0; i < playerCount; i++ )
	{
		pInfo = SERVER->GetPlayerManager()->GetAccountInfo( pPlayer->m_pMainData->SocialItem[i].playerId );
		
		if( pInfo )
		{
			if( pPlayer->m_pMainData->SocialItem[i].friendValue >= 1 )
			{
				pSendHead->Id = pInfo->AccountId;
				pSendHead->DestZoneId = pHead->SrcZoneId;

				SERVER->GetServerSocket()->Send( pInfo->socketId, sendPakcet );
			}
		}
	}
}

	OMP_LOG();
}

void CSocialManager::EVENT_FUNCTION( HandleQueryChearCount )
{
	DO_LOCK( Lock::Player | Lock::Social );

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( pHead->Id );

	if( pPlayerData.isNull() )
		return ;

	int count = 0;

	SERVER->GetPlayerManager()->SendSimplePacket( pPlayerData->BaseData.PlayerId, CLIENT_WORLD_QueryPlayerHotResponse, count );
}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldNoAdded )
{
	DO_LOCK( Lock::Player | Lock::Social );

	int playerId = pHead->Id;

	bool isOpen = pHead->DestZoneId;

	mNoAddedMap[playerId] = isOpen;
}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldFindFriendRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );		

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( pHead->Id );

	if( pPlayerData.isNull() )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, CLIENT_WORLD_QueryFriendResonse, pPlayerData->BaseData.AccountId, SERVICE_CLIENT );

	stSocialInfo info;

	bool isOk = packSingleSocialInfo( info, pPlayerData, queryFriendType( pHead->Id, pHead->DestZoneId ), pHead->DestZoneId, queryFriendValue( pHead->Id, pHead->DestZoneId ), sendPakcet );

	// 如果没有成功，则说明此角色有可能已经被删除
	if( !isOk )
	{
		destoryLink( pHead->Id, pHead->DestZoneId );
	}

	pSendHead->DestZoneId = isOk;
	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPakcet );
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldMakeRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );

	int nError = 0;
	
	nError = makeLink( pHead->Id, pHead->DestZoneId, (SocialType::Type)pHead->SrcZoneId );

	if( nError == SocialError::None )
	{
		SERVER->GetPlayerManager()->SendSimplePacket( pHead->Id, WORLD_CLIENT_SocialMake, pHead->DestZoneId, pHead->SrcZoneId );
	}
	else
	{
		return ;
	}

	nError = makeLink( pHead->DestZoneId, pHead->Id, (SocialType::Type)pHead->SrcZoneId );

	if( nError == SocialError::None )
	{
		SERVER->GetPlayerManager()->SendSimplePacket( pHead->DestZoneId, WORLD_CLIENT_SocialMake, pHead->Id, pHead->SrcZoneId );
	}
	else
	{
		return ;
	}

	SERVER->GetPlayerManager()->SendZoneSimplePacket( pHead->Id, ZONE_WORLD_MakeLinkResponse, pHead->SrcZoneId, pHead->DestZoneId, pHead->SrcZoneId );
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldAddFriendValue )
{
	int player1 = pHead->Id;
	int player2 = pHead->DestZoneId;
	int value = pHead->SrcZoneId;

	addFirendValue(player1,player2,value);

	// HACK: 这里是否需要回应ZoneServer?
}

void CSocialManager::EVENT_FUNCTION( HandleZoneWorldChangeLinkReuquest )
{
	DO_LOCK( Lock::Player | Lock::Social );

	int playerId = pHead->Id;
	int zoneId = pHead->DestZoneId;
	int destPlayerId = Packet->readInt( 32 );
	int oldType = Packet->readInt( 8 );
	int newType = Packet->readInt( 8 );

	int error = 0;

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( playerId );
	if( !pPlayer.isNull() )
	{
		bool isFound = false;
		for( int i = 0; i < pPlayer->m_pMainData->SocialItemCount; i++ )
		{
			if( pPlayer->m_pMainData->SocialItem[i].playerId == destPlayerId && pPlayer->m_pMainData->SocialItem[i].type == oldType )
			{
				isFound = true;
				pPlayer->m_pMainData->SocialItem[i].type = (SocialType::Type)newType;
				break;
			}
		}
	}
	else
	{
		error = -1;
	}
		

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_ChangeLinkResponse, playerId, SERVICE_ZONESERVER, zoneId, destPlayerId, error );

	sendPacket.writeInt( oldType, 8 );
	sendPacket.writeInt( newType, 8 );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}


void CSocialManager::EVENT_FUNCTION( HandleZoneWorldDestoryRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );

	int nError = 0;

	nError = destoryLink( pHead->Id, pHead->DestZoneId );

	if( nError == SocialError::None )
	{
		SERVER->GetPlayerManager()->SendSimplePacket( pHead->Id, WORLD_CLIENT_SocialDestory, pHead->DestZoneId );
	}
	else
	{
		SERVER->GetPlayerManager()->SendZoneSimplePacket( pHead->Id, ZONE_WORLD_DestoryLinkResponse, pHead->SrcZoneId, pHead->DestZoneId, pHead->SrcZoneId, nError );

		return ;
	}

	nError = destoryLink( pHead->DestZoneId, pHead->Id );

	if( nError == SocialError::None )
	{
		SERVER->GetPlayerManager()->SendSimplePacket( pHead->DestZoneId, WORLD_CLIENT_SocialDestory, pHead->Id );
	}
	else
	{
		SERVER->GetPlayerManager()->SendZoneSimplePacket( pHead->Id, ZONE_WORLD_DestoryLinkResponse, pHead->SrcZoneId, pHead->DestZoneId, pHead->SrcZoneId, nError );

		return ;
	}

	SERVER->GetPlayerManager()->SendZoneSimplePacket( pHead->Id, ZONE_WORLD_DestoryLinkResponse, pHead->SrcZoneId, pHead->DestZoneId, pHead->SrcZoneId );
}

// 处理Zone改变玩家状态的请求
void CSocialManager::EVENT_FUNCTION( HandleZoneWorldPlayerStatusChangeRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );

	CLocker ll( m_cs );

	changePlayerStatus( pHead->Id, (PlayerStatusType::Type)pHead->DestZoneId );
}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldChearRequest )
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayerSrc = SERVER->GetPlayerManager()->GetPlayerData( pHead->Id );
	PlayerDataRef pPlayerDest = SERVER->GetPlayerManager()->GetPlayerData( pHead->DestZoneId );
	stAccountInfo* pAccountDest = SERVER->GetPlayerManager()->GetAccountInfo( pHead->DestZoneId );

	if( pPlayerSrc.isNull() )
	{
		// 非法的请求

		return ;
	}

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, CLIENT_WORLD_ChearResponse, pPlayerSrc->BaseData.AccountId, SERVICE_CLIENT, SocialError::None );

	if( pPlayerDest.isNull() )
	{
		pSendHead->Id = SocialError::NotFound;
	}
	else
	{
		int nError = chear( pHead->Id, pHead->DestZoneId );

		if( nError == SocialError::None )
		{
			CMemGuard buf1( MAX_PACKET_SIZE );
			Base::BitStream sendPakcet1( buf.get(), MAX_PACKET_SIZE );
			stPacketHead* pSendHead1 = IPacket::BuildPacketHead( sendPakcet, WORLD_CLIENT_ChearNotify, pPlayerDest->BaseData.AccountId, SERVICE_CLIENT );
			sendPakcet1.writeString( pPlayerDest->BaseData.PlayerName );
			pSendHead1->PacketSize = sendPakcet1.getPosition() - sizeof( stPacketHead );

			if( pAccountDest )
				SERVER->GetServerSocket()->Send( pAccountDest->accountSocketId, sendPakcet1 );
			//SERVER->GetPlayerManager()->SendSimplePacket( pPlayerDest->BaseData.PlayerId, WORLD_CLIENT_ChearNotify, pHead->Id );
		}

		pSendHead->Id = nError;
	}

	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPakcet );
}

void CSocialManager::EVENT_FUNCTION( HandleClientWorldFindPlayerRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );
	
	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( pHead->Id );

	if( !pAccountInfo )
		return ;

	PlayerDataRef pPlayerData;
	if( pHead->DestZoneId == 0 )
	{
		char playerName[COMMON_STRING_LENGTH];
		Packet->readString( playerName,COMMON_STRING_LENGTH );
		std::string s = playerName;
		pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( s );
		if( pPlayerData.isNull() )
			pPlayerData = SERVER->GetPlayerManager()->GetOfflinePlayerData( s );
	}
	else
	{
		pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( pHead->DestZoneId );
		if( pPlayerData.isNull() )
			SERVER->GetPlayerManager()->GetOfflinePlayerData( pHead->DestZoneId );
	}

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, CLIENT_WORLD_FindPlayerResponse, pAccountInfo->AccountId, SERVICE_CLIENT, SocialError::None );

	if( pPlayerData.isNull() )
	{
		pSendHead->DestZoneId = SocialError::NotFound;
	}
	else
	{
		packPlayerSocialInfoList( sendPakcet, pHead->Id, pPlayerData );
	}

	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPakcet );

}

// 处理玩家社会关系列表请求
void CSocialManager::EVENT_FUNCTION( HandleClientWorldSocialInfoRequest )
{
	DO_LOCK( Lock::Player | Lock::Social );
	
	int playerId = pHead->Id;

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( playerId );
	if( !pAccountInfo )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, CLIENT_WORLD_SocialInfoResponse, pAccountInfo->AccountId, SERVICE_CLIENT );

	packSocialInfoList( sendPakcet, playerId );

	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPakcet );
}

void CSocialManager::packSocialInfoList( Base::BitStream& packet, int playerId )
{
	DO_LOCK( Lock::Player );

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( playerId );

	if( pPlayerData.isNull() || !pPlayerData->m_pMainData )
		return ;

	stSocialInfo info;
	int friendPlayer;

	packet.writeInt( pPlayerData->m_pMainData->SocialItemCount, Base::Bit16 );

	for( int i = 0; i < pPlayerData->m_pMainData->SocialItemCount; i++ )
	{
		friendPlayer = pPlayerData->m_pMainData->SocialItem[i].playerId;
		packSingleSocialInfo(info, pPlayerData, (char)pPlayerData->m_pMainData->SocialItem[i].type, friendPlayer, pPlayerData->m_pMainData->SocialItem[i].friendValue, packet);
	}
}

//////////////////////////////////////////////////////////////////////////
// 当玩家登陆到游戏时，需要通知与改玩家有关的玩家，以便这些关系玩
// 家的状态发生变化时通知此玩家
//////////////////////////////////////////////////////////////////////////
void CSocialManager::onPlayerEnter( int playerId )
{
	DO_LOCK( Lock::Player | Lock::Social );

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( playerId );
	PlayerDataRef pp;

	if( pPlayer.isNull() )
		return ;

	for( int i = 0; i < pPlayer->m_pMainData->SocialItemCount; i++ )
	{
		pp = SERVER->GetPlayerManager()->GetPlayerData( pPlayer->m_pMainData->SocialItem[i].playerId );

		if( pp.isNull() )
			pp = SERVER->GetPlayerManager()->GetOfflinePlayerData( pPlayer->m_pMainData->SocialItem[i].playerId );

		if( pp.isNull() )
		{
			for( int j = 0; j < pPlayer->m_pMainData->SocialItemCount - 1;j++ )
			{
				pPlayer->m_pMainData->SocialItem[j].playerId = pPlayer->m_pMainData->SocialItem[j+1].playerId ;
				pPlayer->m_pMainData->SocialItem[j].type= pPlayer->m_pMainData->SocialItem[j+1].type ;
				pPlayer->m_pMainData->SocialItem[j].friendValue = pPlayer->m_pMainData->SocialItem[j+1].friendValue ;
			}
			
			pPlayer->m_pMainData->SocialItemCount--;
		}
	}

	if( pPlayer.isNull() )
		return ;

	for( int i = 0; i < pPlayer->m_pMainData->SocialItemCount; i++ )
	{
		mLinkManMap[ pPlayer->m_pMainData->SocialItem[i].playerId ][playerId] = pPlayer->m_pMainData->SocialItem[i].type;
	}

	// 通知该玩家的所有好友，该玩家上线
	changePlayerStatus( playerId, PlayerStatusType::Idle );	
}

void CSocialManager::onPlayerQuit( int playerId )
{
	DO_LOCK( Lock::Player | Lock::Social );

	// 通知该玩家的所有好友，该玩家下线
	changePlayerStatus( playerId, PlayerStatusType::Offline );

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( playerId );

	if( pPlayer.isNull() || !pPlayer->m_pMainData )
		return ;

	for( int i = 0; i < pPlayer->m_pMainData->SocialItemCount; i++ )
	{
		mLinkManMap[ pPlayer->m_pMainData->SocialItem[i].playerId ].erase( playerId );
	}

	removePlayerFriendTemp( pPlayer );
}

void CSocialManager::notifyPlayerStatusChange( int playerId, int destPlayerId, PlayerStatusType::Type status )
{
	DO_LOCK( Lock::Player | Lock::Social );

	CLocker l( m_cs );

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( playerId );

	stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo( playerId );

	if( pPlayer.isNull() || !pAccount )
		return ;

	CMemGuard buf( 200 );
	Base::BitStream sendPacket( buf.get(), 200 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, WORLD_CLIENT_PlayerStatusChange, pAccount->AccountId, SERVICE_CLIENT, destPlayerId, status );
	
	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccount->socketId, sendPacket );
}

void CSocialManager::changePlayerStatus( int playerId, PlayerStatusType::Type status )
{
	DO_LOCK( Lock::Player | Lock::Social );

	CLocker l( m_cs );

	mStatusMap[playerId] = status;

	LinkManMap::iterator it;

	it = mLinkManMap.find( playerId );

	if( it == mLinkManMap.end() )
		return ;

	LinkMap::iterator lit;

	for( lit = it->second.begin(); lit != it->second.end(); lit++ )
	{
		notifyPlayerStatusChange( lit->first, playerId, status );
	}
}

int CSocialManager::chear( int playerSrc, int playerDest )
{
	DO_LOCK( Lock::Social );

	// 加油冷却时间30分钟
	if( GetTickCount() - mChearMap[playerSrc][playerDest] < 30000 )
		return SocialError::ChearCooldown;
	else
		mChearMap[playerSrc][playerDest] = GetTickCount();

	return SocialError::None;
}

void CSocialManager::packPlayerSocialInfoList( Base::BitStream& sendPakcet, int playerId, PlayerDataRef pPlayerData )
{
	DO_LOCK( Lock::Social | Lock::Player );

	stPlayerSocialInfo info;

	info.id = pPlayerData->BaseData.PlayerId;
	info.level = pPlayerData->DispData.Level;
	strcpy_s( info.name, sizeof( info.name ), pPlayerData->BaseData.PlayerName );
	info.status = mStatusMap[ pPlayerData->BaseData.PlayerId];
	info.family = pPlayerData->DispData.Family;
	info.friendValue = queryFriendValue( pPlayerData->BaseData.PlayerId, playerId );
	
	// TODO: 填充其他信息

	info.WritePakcet( &sendPakcet );

	sendPakcet.writeInt( SERVER->GetTeamManager()->GetTeamId(playerId), 32 );
}

bool CSocialManager::checkFriend( int player1, int player2 )
{
	DO_LOCK( Lock::Social | Lock::Player );

	PlayerDataRef pPlayer1 = SERVER->GetPlayerManager()->GetPlayerData( player1 );
	PlayerDataRef pPlayer2 = SERVER->GetPlayerManager()->GetPlayerData( player2 );

	if( pPlayer1.isNull() || pPlayer2.isNull())
		return false;

	bool isFound = false;

	for( int i = 0; i < pPlayer1->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayer1->m_pMainData->SocialItem[i].playerId == player2 )
		{	
			isFound = true;
			break;
		}
	}

	if( !isFound )
		return false;

	isFound = false;

	for( int i = 0; i < pPlayer2->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayer2->m_pMainData->SocialItem[i].playerId == player1 )
		{	
			isFound = true;
			break;
		}
	}

	if( !isFound )
		return false;

	return true;
}

int CSocialManager::queryFriendValue( int player1, int player2 )
{
	DO_LOCK( Lock::Social | Lock::Player );

	PlayerDataRef pPlayer1 = SERVER->GetPlayerManager()->GetPlayerData( player1 );
	PlayerDataRef pPlayer2 = SERVER->GetPlayerManager()->GetPlayerData( player2 );

	if( pPlayer1.isNull() || pPlayer2.isNull() )
		return 0;
 
	if( !checkFriend( player1, player2 ) )
		return 0;

	for( int i = 0; i < pPlayer1->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayer1->m_pMainData->SocialItem[i].playerId == player2 )
			return pPlayer1->m_pMainData->SocialItem[i].friendValue;
	}

	// this will be never happen	
	return 0;
}

void CSocialManager::addFirendValue( int player1, int player2, int value )
{
	DO_LOCK( Lock::Social | Lock::Player );

	PlayerDataRef pPlayer1 = SERVER->GetPlayerManager()->GetPlayerData( player1 );
	PlayerDataRef pPlayer2 = SERVER->GetPlayerManager()->GetPlayerData( player2 );

	if( pPlayer1.isNull() || pPlayer2.isNull() )
		return ;

	if( !checkFriend( player1, player2 ) )
		return ;
	
	for( int i = 0; i < pPlayer1->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayer1->m_pMainData->SocialItem[i].playerId == player2 )
			pPlayer1->m_pMainData->SocialItem[i].friendValue += value;
	}

	for( int i = 0; i < pPlayer2->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayer2->m_pMainData->SocialItem[i].playerId == player1 )
			pPlayer2->m_pMainData->SocialItem[i].friendValue += value;
	}
}

bool CSocialManager::packSingleSocialInfo( stSocialInfo &info, PlayerDataRef pPlayerData, char type, int friendPlayer, int friendValue, Base::BitStream& packet )
{
	// TODO: fill the info here
	DO_LOCK( Lock::Player | Lock::Social );

	bool isOffline;
	PlayerDataRef pFriendPlayer;
	//	id
	info.id = friendPlayer;

	isOffline = false;

	pFriendPlayer = SERVER->GetPlayerManager()->GetPlayerData( friendPlayer );
	if( pFriendPlayer.isNull() )
	{
		isOffline = true;
		pFriendPlayer = SERVER->GetPlayerManager()->GetOfflinePlayerData( friendPlayer );
	}

	if( !pFriendPlayer.isNull() )
	{
		// name
		strcpy_s( info.name, sizeof( info.name ), pFriendPlayer->BaseData.PlayerName );
		// type
		info.type = (char) type;

		info.level = pFriendPlayer->DispData.Level;

		info.family = pFriendPlayer->DispData.Family;

		info.sex = pFriendPlayer->DispData.Sex;

		info.friendValue = friendValue;

		// status
		if( isOffline )
			info.status = (char)PlayerStatusType::Offline;
		else
			info.status = (char) mStatusMap[ friendPlayer ];
	}
	// write the whole information
	info.WritePakcet( &packet );

	packet.writeInt( SERVER->GetTeamManager()->GetTeamId( friendPlayer ), 32 );

	if( !pFriendPlayer.isNull() )
		return true;
	else
		return false; 
}

char CSocialManager::queryFriendType( int player1, int player2 )
{
	DO_LOCK( Lock::Player | Lock::Social );

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( player1 );
	
	if( pPlayerData.isNull() )
		return 0;

	for( int i = 0; i < pPlayerData->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayerData->m_pMainData->SocialItem[i].playerId == player2 )
			return (char)pPlayerData->m_pMainData->SocialItem[i].type;
	}

	return 0;
}

// 临时好友不保存到数据库
void CSocialManager::removePlayerFriendTemp( PlayerDataRef pPlayer )
{
	DO_LOCK( Lock::Player );

	for( int i = 0; i < pPlayer->m_pMainData->SocialItemCount; i++ )
	{
		if( pPlayer->m_pMainData->SocialItem[i].type == SocialType::Temp )
		{
			for( int j = i; j < pPlayer->m_pMainData->SocialItemCount - 1; j++ )
			{
				pPlayer->m_pMainData->SocialItem[j].type = pPlayer->m_pMainData->SocialItem[j+1].type;
				pPlayer->m_pMainData->SocialItem[j].playerId = pPlayer->m_pMainData->SocialItem[j+1].playerId;
				pPlayer->m_pMainData->SocialItem[j].friendValue = pPlayer->m_pMainData->SocialItem[j+1].friendValue;
			}

			pPlayer->m_pMainData->SocialItemCount--;
		}
	}
}

void CSocialManager::notifyZonePlayerSocialInfo( int playerId )
{
	DO_LOCK( Lock::Player | Lock::Social | Lock::Server );

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( playerId );
	stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo( playerId );

	if( pPlayer.isNull() || !pAccount || !pPlayer->m_pMainData )
		return ;

	int ZoneId = pPlayer->DispData.ZoneId;
	
	if( pPlayer->DispData.CopyMapInstId != 1 )
		ZoneId = pPlayer->DispData.fZoneId;

	int LineId = pAccount->LineId;

	stServerInfo* pServer = SERVER->GetServerManager()->GetGateServerByZoneId( LineId, ZoneId );

	if( !pServer )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPakcet( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPakcet, WORLD_ZONE_SocialInfo, playerId, SERVICE_ZONESERVER, ZoneId );

	sendPakcet.writeInt( pPlayer->m_pMainData->SocialItemCount, 16 );
	for( int i = 0; i < pPlayer->m_pMainData->SocialItemCount; i++ )
	{
		sendPakcet.writeInt( pPlayer->m_pMainData->SocialItem[i].playerId, 32 );
		sendPakcet.writeInt( pPlayer->m_pMainData->SocialItem[i].type, 8 );
		sendPakcet.writeInt( pPlayer->m_pMainData->SocialItem[i].friendValue, 16 );
	}

	pSendHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );


	SERVER->GetServerSocket()->Send( pServer->SocketId, sendPakcet );
}