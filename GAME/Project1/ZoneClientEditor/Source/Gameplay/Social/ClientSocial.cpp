#include "Gameplay/ClientGameplayState.h"
#include "Network/UserPacketProcess.h"
#include "ClientSocial.h"
#include "Network/EventCommon.h"
#include "common/SocialBase.h"
#include "../ClientPlayerManager.h"
#include "../GameEvents/ClientGameNetEvents.h"
#include "zlib/zlib.h"

CClientSocial g_clientSocial;
EventMethod gEventMethod[END_NET_MESSAGE];

// 加为好友
void CClientSocial::makeFriend( int playerId, int destPlayerId, bool isTemp )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead(sendPacket, CLIENT_WORLD_MakeLinkRequest, playerId, SERVICE_WORLDSERVER, destPlayerId, isTemp );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );
}

// 解除好友
void CClientSocial::destoryFriend( int playerId, int destPlayerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_DestoryLinkRequest, playerId, SERVICE_WORLDSERVER, destPlayerId );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );
}


CClientSocial::CClientSocial()
{
	mQueryPrenticeNum = 0;
}

void CClientSocial::init()
{
	gEventMethod[CLIENT_WORLD_MakeLinkResponse]			= &CClientSocial::HandleClientWorldMakeLinkResponse;
	gEventMethod[CLIENT_WORLD_DestoryLinkResponse]		= &CClientSocial::HandleClientWorldDestoryLinkResponse; 
	gEventMethod[CLIENT_WORLD_ChearResponse]			= &CClientSocial::HandleClientWorldChearResponse;
	gEventMethod[CLIENT_WORLD_SocialInfoResponse]		= &CClientSocial::HandleClientWorldSocialListResponse;
	gEventMethod[WORLD_CLIENT_ChearNotify]				= &CClientSocial::HandleWorldClientChearNotify;
	gEventMethod[WORLD_CLIENT_SocialAdded]				= &CClientSocial::HandleWorldClientSocialAdded;
	gEventMethod[WORLD_CLIENT_SocialRemoved]			= &CClientSocial::HandleWorldClientSocialRemoved;
	gEventMethod[CLIENT_WORLD_FindPlayerResponse]		= &CClientSocial::HandleClientWorldFindPlayerResponse;
	gEventMethod[CLIENT_WORLD_QueryFriendResonse]		= &CClientSocial::HandleClientWorldQueryFriendResponse;
	gEventMethod[WORLD_CLIENT_PlayerStatusChange]		= &CClientSocial::HandlePlayerStatusChange;
	gEventMethod[CLIENT_WORLD_QueryPlayerHotResponse]	= &CClientSocial::HandleClientWorldQueryPlayerHotResponse;
	gEventMethod[CLIENT_WORLD_QueryChearCountResponse]	= &CClientSocial::HandleClientWorldQueryChearChoutResponse;
	gEventMethod[WORLD_CLIENT_EncourageNotify]			= &CClientSocial::HandleWorldClientEncourageNotify;
	gEventMethod[CLIENT_WORLD_PlayerInfoResponse]		= &CClientSocial::HandleClientWorldPlayerInfoResponse;
}

CClientSocial::~CClientSocial()
{

}

void CClientSocial::HandleClientWorldMakeLinkResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	int nError = pHead->DestZoneId;
	
	if( nError == 0 )
	{
		sendQueryFriend( g_ClientGameplayState->getSelectedPlayerId(), pHead->SrcZoneId );
	}
	// TODO: handle response
}

void CClientSocial::HandleClientWorldDestoryLinkResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	int nError = pHead->DestZoneId;
	int playerId = pHead->SrcZoneId;

	stSocialInfo& info = mSocialMap[playerId];

	// TODO: handle response
	Con::executef("RemoveFriendList",Con::getIntArg(playerId));
	Con::executef( "AddFriendList","4",Con::getIntArg(info.id), info.name,Con::getIntArg(info.level),Con::getIntArg(info.family),Con::getIntArg(info.type),Con::getIntArg(info.status));
}

void CClientSocial::HandleClientWorldChearResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	INT nError = pHead->DestZoneId;

	// TODO: handle response
}

void CClientSocial::HandlePlayerStatusChange( stPacketHead* pHead, Base::BitStream& packet )
{
	INT playerId = pHead->DestZoneId;
	int status = pHead->SrcZoneId;

	SocialMap::iterator it;
	it = mSocialMap.find(playerId);
	if( it == mSocialMap.end() )
		return ;
	
	stSocialInfo& info = it->second;

	info.status = status;
	Con::executef( "RemoveFriendList", Con::getIntArg(info.id) );
	Con::executef( "AddFriendList","1",Con::getIntArg(info.id), info.name,Con::getIntArg(info.level),Con::getIntArg(info.family),Con::getIntArg(info.type),Con::getIntArg(info.status));
	// TODO: handle response
}
void CClientSocial::chearFriend( int playerId, int destPlayerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_ChearRequest, playerId, SERVICE_WORLDSERVER, destPlayerId );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );	
}

void CClientSocial::sendQueryFriend( int playerId, int destPlayerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_QueryFriendRequest, playerId, SERVICE_WORLDSERVER, destPlayerId );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );	
}

void CClientSocial::HandleClientWorldSocialListResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	int size = packet.readInt( 16 );
	
	stSocialInfo info;

	mSocialMap.clear();
	Con::executef("ClearFriendList");

	for( int i=0; i < size; i++ )
	{
		info.ReadPacket( &packet );
		int teamId = packet.readInt( 32 );

		stPlayerDisp& playerDisp = PLAYER_MGR->GetPlayerDisp( info.id );
		std::string name = info.name;
		PLAYER_MGR->SetPlayerIdByName( name, info.id );
		playerDisp.Sex = info.sex;
		playerDisp.Family = info.family;
		playerDisp.Level = info.level;
		playerDisp.TeamId = teamId;

		mSocialMap[info.id] = info;
		Con::executef( "AddFriendList","1",Con::getIntArg(info.id), info.name,Con::getIntArg(info.level),Con::getIntArg(info.family),Con::getIntArg(info.type),Con::getIntArg(info.status));
	}
	
	// here update the UI
}

void CClientSocial::HandleClientWorldQueryFriendResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	int nError = pHead->DestZoneId;

	stSocialInfo info;
	info.ReadPacket( &packet );
	int teamId = packet.readInt( 32 );

	stPlayerDisp& playerDisp = PLAYER_MGR->GetPlayerDisp( info.id );
	std::string name = info.name;
	PLAYER_MGR->SetPlayerIdByName( name, info.id );
	playerDisp.Sex = info.sex;
	playerDisp.Family = info.family;
	playerDisp.Level = info.level;
	playerDisp.TeamId = teamId;

	// here update the UI
	Con::executef( "AddFriendList","1",Con::getIntArg(info.id), info.name,Con::getIntArg(info.level),Con::getIntArg(info.family),Con::getIntArg(info.type),Con::getIntArg(info.status));
}

void CClientSocial::sendSocialRequest( int playerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_SocialInfoRequest, playerId, SERVICE_WORLDSERVER );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );	
}

void CClientSocial::HandleClientWorldQueryPlayerHotResponse(stPacketHead* pHead, Base::BitStream& packet)
{
	int value = pHead->DestZoneId;

	// TODO: update the UI
}
void CClientSocial::HandleWorldClientEncourageNotify(stPacketHead* pHead, Base::BitStream& packet)
{
	int playerId = pHead->Id;
	int BuffId = pHead->DestZoneId;

	// TODO: update the UI
}

void CClientSocial::HandleClientWorldQueryChearChoutResponse(stPacketHead* pHead, Base::BitStream& packet)
{
	int value = pHead->DestZoneId;

	// TODO: update the UI
}

void CClientSocial::HandleWorldClientChearNotify( stPacketHead* pHead, Base::BitStream& packet )
{
	int playerId = pHead->DestZoneId;

	// TODO: show the player cheared
}

void CClientSocial::HandleWorldClientSocialAdded( stPacketHead* pHead, Base::BitStream& packet )
{
	// when the player make a friend via zone server, the world server will notify the client

	int playerId = pHead->DestZoneId;
	int type = pHead->SrcZoneId;

	PLAYER_MGR->HandleInfoPacket( &packet );

	// 更新UI，显示有人加自己为好友
	Con::printf("玩家[%s]加你为好友！", PLAYER_MGR->getPlayerName( playerId ).c_str() );

	//stSocialInfo socialInfo;

	//socialInfo.ReadPacket( &packet );

	//mSocialMap[socialInfo.id]  = socialInfo;

	// TODO: update the UI


	// 创建本地逻辑延迟事件
	Player::stLogicEvent* logicEvent = new Player::stLogicEvent;
	logicEvent->type		= Player::LOGICEVENT_FRIENDINVITE;
	logicEvent->sender		= playerId;
	logicEvent->senderName	= StringTable->insert( PLAYER_MGR->getPlayerName( playerId ).c_str() );
	logicEvent->data		= NULL;
	logicEvent->datalen		= 0;

	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if( pPlayer )
	{	
		U32 eid = pPlayer->insertLogicEvent(logicEvent);

		// 弹出通用邀请计时对话框
		Con::executef("CommonInviteDialog", Con::getIntArg(eid));
	}
}

void CClientSocial::HandleWorldClientSocialRemoved( stPacketHead* pHead, Base::BitStream& packet )
{
	int playerId = pHead->DestZoneId;

	mSocialMap.erase( playerId );

	// TODO: update the UI
}

void CClientSocial::sendNoAddedRequest( int playerId, int isOpen )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_CLIENT_SocialNoAddedRequest, playerId, SERVICE_WORLDSERVER, isOpen );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CClientSocial::sendQueryPlayerHotRequest( int playerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_QueryPlayerHotRequest, playerId, SERVICE_WORLDSERVER );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CClientSocial::sendQueryChearCountRequest( int playerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_QueryChearCountRequest, playerId, SERVICE_WORLDSERVER );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );
}


void CClientSocial::HandleClientWorldFindPlayerResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	stPlayerSocialInfo info;
	stSocialInfo sinfo;

	if (pHead->DestZoneId == SocialError::NotFound)
	{
		return;
	}
	else
	{
		info.ReadPacket( &packet );

		sinfo.family = info.family;
		sinfo.id = info.id;
		sinfo.level = info.level;
		strcpy_s( sinfo.name, 32, info.name );
		sinfo.sex = info.sex;

		int teamId = packet.readInt( 32 );

		stPlayerDisp& playerDisp = PLAYER_MGR->GetPlayerDisp( info.id );
		std::string name = info.name;
		PLAYER_MGR->SetPlayerIdByName( name, info.id );
		playerDisp.Sex = info.sex;
		playerDisp.Family = info.family;
		playerDisp.Level = info.level;
		playerDisp.TeamId = teamId;

		// TODO: update the UI

		int action = atoi( Con::getVariable("$SocialFindPlayerAction") );

		switch( action )
		{
		case 1:
			makeFriend( g_ClientGameplayState->getSelectedPlayerId(), info.id, false );
			mSocialMap[sinfo.id] = sinfo;
			break;
		case 2:
			Con::executef("ShowPlayerInfoInRelationWnd",Con::getIntArg(info.id),(const char*)info.name,Con::getIntArg(info.level),Con::getIntArg(info.soulLevel),Con::getIntArg(info.friendValue),Con::getIntArg( info.type ),(const char*)info.mapName,Con::getIntArg(info.family),(const char*)info.groupName);
			break;
		case 3:
			Con::executef("ReadyToInvationTeam", info.status !=0 ? Con::getIntArg(info.id) : "0" );
			break;
		}
		
		Con::setVariable("$SocialFindPlayerAction","0");
	}
}

void CClientSocial::findPlayerByName( int playerId, const char* playerName )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_FindPlayerRequest, playerId, SERVICE_WORLDSERVER );

	sendPacket.writeString( playerName );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );		
}

void CClientSocial::findPlayerById( int playerId, int destPlayerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_FindPlayerRequest, playerId, SERVICE_WORLDSERVER, destPlayerId );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ClientGameplayState->GetPacketProcess()->Send( sendPacket );	
}

void CClientSocial::cleanup()
{
	// here we clean up the UI
	Con::executef("SptSocialRequest");
}

//---------------------------------------------------------------
// 响应玩家信息请求
//---------------------------------------------------------------
void CClientSocial::HandleClientWorldPlayerInfoResponse( stPacketHead* pHead, Base::BitStream& packet )
{
	bool ok = packet.readFlag();
 
	if( ok )
	{
		//PlayerStatusType::Type  statusType= packet.readInt(4);
		int statusType = 1;//暂用
		int size = packet.readInt( 20 );
		CMemGuard zipBuf( size );
		packet.readBits( size * 8, zipBuf.get() );
		CMemGuard dataBuf( MAX_PACKET_SIZE );
		int dSize = MAX_PACKET_SIZE;
		uncompress( (Bytef*)dataBuf.get(), (uLongf*)&dSize, (Bytef*)zipBuf.get(), size );
		Base::BitStream dataStream( dataBuf.get(), dSize );
		stPlayerStruct player;
		player.ReadData( &dataStream );

		if(player.MainData.MasterLevel>0)
		{

			for (int i=0; i<player.MainData.SocialItemCount; i++)
			{
				if(player.MainData.SocialItem[i].type == SocialType::Prentice)
				{
					mQueryPrenticeNum++;
					sendPlayerInfoRequest(player.MainData.SocialItem[i].playerId);
				}
			}

			U8 maxPrenNum = MPManager::getData(player.MainData.MasterLevel).mMaxPrenticeNum;
			Con::executef("uiUpdateMasterInfo",
				player.BaseData.PlayerName,
				Con::getIntArg(mQueryPrenticeNum),
				Con::getIntArg(maxPrenNum),
				player.MainData.MasterText);
		}
		else if(player.MainData.bPrentice)
		{
			if(mQueryPrenticeNum>0)
			{
				Con::executef("uiAddPrenticeList",
					Con::getIntArg(player.BaseData.PlayerId),
					player.BaseData.PlayerName,
					Con::getIntArg(player.DispData.Level),
					Con::getIntArg(player.DispData.Family),
					Con::getIntArg(statusType));
				mQueryPrenticeNum--;
				return;
			}

			if(statusType == PlayerStatusType::Offline)//不在线
			{
				Platform::LocalTime lt;
				Platform::getDateTime(player.BaseData.lastLogoutTime,lt);
				char szTime[128] = {0,};
				dSprintf(szTime,sizeof(szTime),"4%d-2d-2d  2%d:2%d",lt.year+1900,lt.month+1,lt.monthday,lt.hour,lt.min);
				Con::executef("offlinePrenticeInfo",player.BaseData.PlayerName,szTime);
			}
			else                                       //在线
			{
				S32 friendValue = 0;
				for (int i =0; i< player.MainData.SocialItemCount; i++)
				{
					if(player.MainData.SocialItem[i].type = SocialType::Master)
					{
						friendValue = player.MainData.SocialItem[i].friendValue;
						break;
					}
				}

				Con::executef("onlinePrenticeInfo",
					player.BaseData.PlayerName,
					Con::getIntArg(player.MainData.MPMStudyLevel),
					Con::getIntArg(friendValue),
					"0/0");
			}
		}
	}
	else
	{
		// 玩家没找到或者数据错误
	}
}

void CClientSocial::sendPlayerInfoRequest( int playerId )
{
	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if( !pPlayer )
		return ;
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_PlayerInfoRequest, g_ClientGameplayState->getAccountId(),  SERVICE_WORLDSERVER, playerId );
	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}
// 发送自动拒绝请求
ConsoleFunction( SptSocialSendNoAdded, void, 2, 2, "" )
{
	g_clientSocial.sendNoAddedRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 通过id查找好友
ConsoleFunction( SptSocialFindPlayerById, void, 2, 2, "" )
{
	g_clientSocial.findPlayerById( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 通过名字查找好友
ConsoleFunction( SptSocialFindPlayerByName, void, 2, 2, "" )
{
	g_clientSocial.findPlayerByName( g_ClientGameplayState->getSelectedPlayerId(), argv[1] ); 
}

// 通知不要别人加自己
ConsoleFunction( SptSocialNoAdded, void, 2, 2, "" )
{
	g_clientSocial.sendNoAddedRequest( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1]) );
}

// 给别人加油
ConsoleFunction( SptSocialChear, void, 2, 2, "" )
{
	g_clientSocial.chearFriend( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 加为好友
ConsoleFunction( SptSocialMakeFirend, void, 2, 2, "" )
{
	g_clientSocial.makeFriend( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ), false );
}

// 加为临时好友
ConsoleFunction( SptSocialMakeFirendTemp, void, 2, 2, "" )
{
	g_clientSocial.makeFriend( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 删除好友
ConsoleFunction( SptSocialDestoryFriend, void, 2, 2, "" )
{
	g_clientSocial.destoryFriend( g_ClientGameplayState->getSelectedPlayerId(), atoi( argv[1] ) );
}

// 请求好友列表
ConsoleFunction( SptSocialRequest , void, 1, 1, "" )
{
	g_clientSocial.sendSocialRequest(g_ClientGameplayState->getSelectedPlayerId());
}

// 请求仙缘
ConsoleFunction( SptSocialQueryPlayerHot, void, 1, 1, "" )
{
	g_clientSocial.sendQueryPlayerHotRequest( g_ClientGameplayState->getSelectedPlayerId() );
}

// 请求加油数
ConsoleFunction( SptSocialQueryChearCount, void, 1, 1, "" )
{
	g_clientSocial.sendQueryChearCountRequest( g_ClientGameplayState->getSelectedPlayerId() );
}

// 卜卦
ConsoleFunction( SptSocialGetEncourage, void, 1,1, "" )
{
	GameConnection* conn = dynamic_cast<GameConnection*>(GameConnection::getConnectionToServer());
	if (conn)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent( INFO_GETENCOURAGE );
		if( pEvent )
			conn->postNetEvent( pEvent );
	}	
}

ConsoleFunction( OnEncourageResult, void, 2,2, "" )
{
	Con::printf( "卜卦结果:%s", argv[1] );
}