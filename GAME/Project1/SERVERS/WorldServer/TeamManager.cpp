#include "TeamManager.h"
#include "ChatMgr.h"
#include "WorldServer.h"
#include "SocialManager.h"

int CTeamManager::m_nTeamIdSeed = 1;

CTeamManager::CTeamManager(void)
{
    registerEvent(CLIENT_TEAM_BuildAlongRequest,		 &CTeamManager::HandleBuildAlongRequest);
    registerEvent(CLIENT_TEAM_BuildRequest,				 &CTeamManager::HandleBuildRequest);
    registerEvent(CLIENT_TEAM_AcceptBuildRequest,		 &CTeamManager::HandleAcceptBuildRequest);
    registerEvent(CLIENT_TEAM_JoinRequest,				 &CTeamManager::HandleJoinRequest);
    registerEvent(CLIENT_TEAM_AcceptdJoinRequest,	     &CTeamManager::HandleAcceptJoinRequest);
    registerEvent(CLIENT_TEAM_AddRequest,				 &CTeamManager::HandleAddRequest);
    registerEvent(CLIENT_TEAM_AcceptAddRequest,			 &CTeamManager::HandleAcceptAddRequest);
    registerEvent(CLIENT_TEAM_LeaveRequest,				 &CTeamManager::HandleLeaveRequest);
    registerEvent(CLIENT_TEAM_MapMark,					 &CTeamManager::HandleMapMark);
    registerEvent(CLIENT_TEAM_DropRequest,				 &CTeamManager::HandleDropRequest);
    registerEvent(CLIENT_TEAM_TargetMark,				 &CTeamManager::HandleTargetMark);
    registerEvent(CLIENT_TEAM_BaseInfo,					 &CTeamManager::HandleBaseInfo);
    registerEvent(CLIENT_TEAM_Disband,					 &CTeamManager::HandleDisband);
    registerEvent(CLIENT_TEAM_Refused,					 &CTeamManager::HandleRefused);
    registerEvent(CLIENT_TEAM_ShowTeamInfo,				 &CTeamManager::HandleInfoRequest);
	registerEvent(CLIENT_TEAM_Follow,					 &CTeamManager::HandleFollowRequest);
	registerEvent(CLIENT_TEAM_InvationCopymap,			 &CTeamManager::HandleInvationCopymap);

	gLock.registerLockable( this, Lock::Team );
}

int CTeamManager::BuildTeamId()
{
	return m_nTeamIdSeed++;
}

int CTeamManager::GetTeamId( int nPlayerId )
{
	DO_LOCK( Lock::Team );

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( pTeam )
		return pTeam->GetId();

	return 0;
}


int CTeamManager::BuildTeam( int nPlayerId1, int nPlayerId2, int& nTeamId )
{
	DO_LOCK( Lock::Team | Lock::Player | Lock::Chat | Lock::Channel );

	int nRet = CheckBuildTeam( nPlayerId1, nPlayerId2 );

	if( nRet != NONE_ERROR )
		return nRet;

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId1 );

	if( pPlayerData.isNull() )
		return UNKNOW_ERROR;

	CTeam* pTeam;
	CMemPool::GetInstance()->AllocObj<CTeam>( pTeam );
	pTeam->Join( nPlayerId1 );
	pTeam->Join( nPlayerId2 );
	nTeamId = BuildTeamId();
	pTeam->m_nId = nTeamId;
	CMemGuard buf(	MAX_TEAM_NAME_LENGTH );
	sprintf( buf.get(), "%s的队伍", pPlayerData->BaseData.PlayerName );
	strcpy( pTeam->m_szName, buf.get() );

	m_playerTeamMap[nPlayerId1] = pTeam;
	m_playerTeamMap[nPlayerId2] = pTeam;
	m_teamMap[nTeamId] = pTeam;

	m_addTeamRecver[nPlayerId2].erase( nPlayerId1 );
	m_addTeamRecver[nPlayerId1].erase( nPlayerId2 );

#ifdef WORLDSERVER

	// 注册一个聊天队伍频道
	CChannel* pChannel = SERVER->GetChatManager()->GetChannelManager()->RegisterChannel( CHAT_MSG_TYPE_TEAM, "" );
	int nChannelId = pChannel->GetId();
	pTeam->m_nChatChannelId = nChannelId;

	// 把创建队伍的两个玩家加入到聊天频道中
	SERVER->GetChatManager()->GetChannelManager()->AddPlayer( nPlayerId1, nChannelId );
	SERVER->GetChatManager()->GetChannelManager()->AddPlayer( nPlayerId2, nChannelId );

	// 暂时从聊天频道中移除玩家, 等待玩家主动加入到聊天频道中
	SERVER->GetChatManager()->GetChannelManager()->RemovePlayer( nPlayerId1, nChannelId, false );
	SERVER->GetChatManager()->GetChannelManager()->RemovePlayer( nPlayerId2, nChannelId, false );

#endif

	return nRet;
}

int CTeamManager::CheckBuildTeam( int nPlayerId1, int nPlayerId2 )
{
	if( nPlayerId1 == nPlayerId2 )
		return TEAM_SELF_ERROR;

	if( m_joinTeamRecver[nPlayerId2].find(nPlayerId1) != m_joinTeamRecver[nPlayerId2].end() )
		return TEAM_ERROR;

	if( m_joinTeamRecver[nPlayerId2].size() == MAX_TEAM_PENDING_COUNT )
		return TEAM_NOTIFY_LIST_FULL;

	stdext::hash_map<int,CTeam*>::iterator it;

	it = m_playerTeamMap.find( nPlayerId1 );
	if( it != m_playerTeamMap.end() )
		return TEAM_SELF_HAVE_TEAM;

	it = m_playerTeamMap.find( nPlayerId2 );
	if( it != m_playerTeamMap.end() )
		return TEAM_SELF_HAVE_TEAM;

	return NONE_ERROR;
}

int CTeamManager::CheckJoinTeam( int nPlayerId )
{
	CTeam* pTeam = GetTeamByPlayer( nPlayerId );

	if( pTeam != NULL )
		return TEAM_SELF_HAVE_TEAM;

	return NONE_ERROR;
}

CTeam* CTeamManager::GetTeamByPlayer( int nPlayerId )
{
	stdext::hash_map<int,CTeam*>::iterator it;

	it = m_playerTeamMap.find( nPlayerId );
	if( it != m_playerTeamMap.end() )
		return it->second;

	return NULL;
}

CTeam* CTeamManager::GetTeam( int nTeamId )
{
	stdext::hash_map<int,CTeam*>::iterator it;

	it = m_teamMap.find( nTeamId );
	if( it != m_teamMap.end() )
		return it->second;

	return NULL;
}

int CTeamManager::JoinTeam( int nPlayerId, int nTeamId )
{
	DO_LOCK( Lock::Team );

	int nRet = CheckJoinTeam( nPlayerId );

	if( nRet != NONE_ERROR )
		return nRet;

	CTeam* pTeam = GetTeam( nTeamId );
	if( pTeam == NULL )
		return -1;

	ClearPendingList(nPlayerId);

#ifdef WORLDSERVER

	// 将新玩家加入到聊天频道中
	SERVER->GetChatManager()->GetChannelManager()->AddPlayer( nPlayerId, pTeam->GetChatChannelId() );
	SERVER->GetChatManager()->GetChannelManager()->RemovePlayer( nPlayerId, pTeam->GetChatChannelId(), false );

#endif

	m_playerTeamMap[ nPlayerId ] = pTeam;
	return pTeam->Join( nPlayerId );
}

int CTeamManager::LeaveTeam( int nPlayerId, int nTeamId )
{
	CTeam* pTeam = GetTeam( nTeamId );
	if( pTeam == NULL )
		return -1;

	if( !pTeam->IsTeammate( nPlayerId ) )
		return -1;

	ClearPendingList( nPlayerId );

#ifdef WORLDSERVER

	// 将玩家移除聊天频道 
	SERVER->GetChatManager()->GetChannelManager()->RemovePlayer( nPlayerId, pTeam->GetChatChannelId() );

#endif

	DO_LOCK( Lock::Team );

	m_playerTeamMap.erase( nPlayerId );

	return pTeam->Leave( nPlayerId );
}


int CTeamManager::DisbandTeam( int nTeamId )
{
	CTeam* pTeam = GetTeam(nTeamId);

	if( pTeam == NULL )
		return -1;

#ifdef WORLDSERVER

	// 移除这个队伍的聊天频道
	SERVER->GetChatManager()->GetChannelManager()->UnregisterChannel( pTeam->GetChatChannelId() );

#endif

	DO_LOCK( Lock::Team | Lock::Player );

	//ClearPendingList( pTeam->GetCaption() );

	stAccountInfo* pAccountInfo;

	// 将所有玩家移除队伍
	std::list<int> pPlayerSet = pTeam->m_teammate;

	std::list<int>::iterator it;

	for( it = pPlayerSet.begin(); it != pPlayerSet.end(); it++ )
	{
		NotifyPlayerLeave( *it, *it );
		pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( *it );
		if( pAccountInfo )
			NotifyZonePlayerLeave( pAccountInfo->LineId, *it, pTeam->GetId() );
		LeaveTeam( *it, pTeam->GetId() );
	}

	m_teamMap.erase( nTeamId );

	CMemPool::GetInstance()->FreeObj<CTeam>( pTeam );

	return NONE_ERROR;	
}

int CTeamManager::DisbandTeamByPlayer( int nPlayerId )
{
	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( pTeam == NULL )
		return -1;

	return DisbandTeam( pTeam->m_nId );
}


void CTeamManager::OnEvent(CEventSource* pEventSouce, const stEventArg& eventArg)
{
	// 此处需要处理玩家离线事件，如要让该玩家离开队伍，如果是队长则解散这个队伍
	// 暂时不处理，留下接口
}


int CTeam::GetCaption()
{
	return *m_teammate.begin();
}

std::list<int>::iterator CTeam::FindTeammate(int nPlayer)
{
	std::list<int>::iterator it;
	for( it = m_teammate.begin(); it != m_teammate.end(); it++ )
	{
		if( *it == nPlayer )
			break;
	}
	return it;
}

void CTeam::SetCaption( int nPlayerId )
{
	if( !IsTeammate( nPlayerId ) )
		return ;

	std::list<int>::iterator it;

	it = FindTeammate( nPlayerId );

	*it = *m_teammate.begin();

	*m_teammate.begin() = nPlayerId;
}

#ifdef WORLDSERVER
int CTeam::GetChatChannelId()
{
	return m_nChatChannelId;
}
#endif


void CTeam::SetName( const char* str )
{
	strcpy( m_szName, str );
}

void CTeam::SetCopymap( int copyMapId )
{
	m_nCopymapId = copyMapId;
}

int CTeam::GetId()
{
	return m_nId;
}

const char* CTeam::GetName()
{
	return m_szName;
}

bool CTeam::IsCaption( int nPlayerId )
{
	return *(m_teammate.begin()) == nPlayerId;
}

bool CTeam::IsTeammate( int nPlayerId )
{
	std::list<int>::iterator it;

	for( it = m_teammate.begin(); it != m_teammate.end(); it++ )
	{
		if (*it == nPlayerId )
			return true;
	}

	return false;

}

int CTeam::Join( int nPlayerId )
{
	if( IsTeammate( nPlayerId ) )
		return -1;

	DO_LOCK( Lock::Social );

	SERVER->GetSocialManager()->changePlayerStatus( nPlayerId, PlayerStatusType::Team );
		
	m_teammate.push_back( nPlayerId );

	return NONE_ERROR;
}

int CTeam::Leave( int nPlayerId )
{
	if( !IsTeammate( nPlayerId ) )
		return -1;

	std::list<int>::iterator it;

	for( it = m_teammate.begin(); it != m_teammate.end(); it++ )
	{
		if (*it == nPlayerId )
			break;
	}

	if( it == m_teammate.end() )
		return NONE_ERROR;
	
	m_teammate.erase( it );

	DO_LOCK( Lock::Social );

	SERVER->GetSocialManager()->changePlayerStatus( nPlayerId, PlayerStatusType::Idle );

	return NONE_ERROR;
}

std::list<int> * CTeam::GetTeammate()
{
	return &m_teammate;
}

int CTeam::GetCopymap()
{
	return m_nCopymapId;
}

void CTeamManager::GetPlayerTeamInfo( int nPlayerId, stTeamInfo& teamInfo )
{
	DO_LOCK( Lock::Team );

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( pTeam )
	{
		teamInfo.m_bInTeam = true;
		teamInfo.m_nId = pTeam->GetId();
		strcpy( teamInfo.m_szName, pTeam->GetName() );
		teamInfo.m_bIsCaption = pTeam->IsCaption( nPlayerId );
	}
}

// 通知队伍中的玩家，此队伍有玩家加入
void CTeamManager::NotifyPlayerJoined( int nPlayerId, int nPlayerIdTo, int nTeamId )
{
	SERVER->GetPlayerManager()->SendSimpleInfoPacket( nPlayerId, CLIENT_TEAM_PlayerJoined, nPlayerIdTo, nTeamId );
}

// 通知队伍中的玩家，此队伍中有玩家退出
void CTeamManager::NotifyPlayerLeave( int nPlayerId,int nPlayerIdTo )
{
	SERVER->GetPlayerManager()->SendSimplePacket( nPlayerId, CLIENT_TEAM_PlayerLeave, nPlayerIdTo );
}

// 通知地图,此队伍有玩家加入
void CTeamManager::NotifyZonePlayerJoined( int nLineId, int nPlayerId, int nTeamId )
{
	DO_LOCK( Lock::Team | Lock::Player | Lock::Server );

	CTeam* pTeam = GetTeam( nTeamId );
	if( !pTeam )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_PlayerJoined, 0, SERVICE_ZONESERVER );
	std::list<int>::iterator it;

	pHead->Id = pTeam->GetId();
	pHead->SrcZoneId = nPlayerId;

	stdext::hash_map<int,stServerInfo*> serverMap;

	PlayerDataRef pPlayer;

	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		pPlayer = SERVER->GetPlayerManager()->GetPlayerData( *it );

		int ZoneId = pPlayer->DispData.ZoneId;
		if( pPlayer->DispData.CopyMapInstId != 1 )
			ZoneId = pPlayer->DispData.fZoneId;

		serverMap[ ZoneId ] = SERVER->GetServerManager()->GetGateServerByZoneId( nLineId, ZoneId );
	}

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	stdext::hash_map<int,stServerInfo*>::iterator itt;

	for( itt = serverMap.begin(); itt != serverMap.end(); itt++ )
	{
		pHead->DestZoneId = itt->first;
		if( itt->second )
			SERVER->GetServerSocket()->Send( itt->second->SocketId, SendPacket );
	}
}

void CTeamManager::NotifyZonePlayerLeave( int nLineId, int nPlayerId, int nTeamId )
{
	DO_LOCK( Lock::Team | Lock::Player | Lock::Server );

	CTeam* pTeam = GetTeam( nTeamId );
	if( !pTeam )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_PlayerLeave, 0, SERVICE_ZONESERVER );
	std::list<int>::iterator it;


	pHead->Id = pTeam->GetId();
	pHead->SrcZoneId = nPlayerId;

	stdext::hash_map<int,stServerInfo*> serverMap;

	PlayerDataRef pPlayer;

	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		pPlayer = SERVER->GetPlayerManager()->GetPlayerData( *it );

		int ZoneId = pPlayer->DispData.ZoneId;
		if( pPlayer->DispData.CopyMapInstId != 1 )
			ZoneId = pPlayer->DispData.fZoneId;

		serverMap[ ZoneId ] = SERVER->GetServerManager()->GetGateServerByZoneId( nLineId, ZoneId );	}

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	stdext::hash_map<int,stServerInfo*>::iterator itt;

	stServerInfo* pInfo;

	for( itt = serverMap.begin(); itt != serverMap.end(); itt++ )
	{
		pHead->DestZoneId = itt->first;
		pInfo = itt->second;
		AssertFatal( pInfo != NULL, "Error" );
		if( pInfo )
			SERVER->GetServerSocket()->Send( pInfo->SocketId, SendPacket );
	}
}


// 发送错误信息
void CTeamManager::SendError( int nPlayerId, int nAck )
{
	SERVER->GetPlayerManager()->SendSimplePacket( nPlayerId, CLIENT_TEAM_Error, nAck );
}

// 发送成员列表
void CTeamManager::SendTeamInfo( int nPlayerId )
{
	DO_LOCK( Lock::Team | Lock::Player );

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( !pTeam )
		return ;

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );

	if( !pAccountInfo )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_TeamInfo, pAccountInfo->AccountId, SERVICE_CLIENT );
	std::list<int>::iterator it;

	pHead->DestZoneId = pTeam->GetId();

	SendPacket.writeFlag( pTeam->m_bIsSquad );
	SendPacket.writeString( pTeam->m_szName, MAX_TEAM_NAME_LENGTH );
	SendPacket.writeInt( pTeam->m_teammate.size(), Base::Bit32 );

	PlayerDataRef pPlayerData;
	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		SendPacket.writeInt( *it, Base::Bit32 );
		pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( *it );
		if( SendPacket.writeFlag( !pPlayerData.isNull() ) )
		{
			SendPacket.writeString( pPlayerData->BaseData.PlayerName );
			SendPacket.writeInt( pPlayerData->DispData.Level, 8 );
			SendPacket.writeInt( pPlayerData->DispData.Sex, 2 );
			SendPacket.writeInt( pPlayerData->DispData.Family, 8 ); 
		}
	}

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );

}

void CTeamManager::SendTeamInfoToZone( int nLineId, int nTeamId )
{
	DO_LOCK( Lock::Team | Lock::Player | Lock::Server );

	CTeam* pTeam = GetTeam( nTeamId );
	if( !pTeam )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_TeamInfo, 0, SERVICE_ZONESERVER );
	std::list<int>::iterator it;

	pHead->Id = pTeam->GetId();

	SendPacket.writeFlag( pTeam->m_bIsSquad );
	SendPacket.writeString( pTeam->m_szName, MAX_TEAM_NAME_LENGTH );
	SendPacket.writeInt( pTeam->m_teammate.size(), Base::Bit32 );

	stdext::hash_map<int,stServerInfo*> serverMap;

	PlayerDataRef pPlayer;

	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		SendPacket.writeInt( *it, Base::Bit32 );

		pPlayer = SERVER->GetPlayerManager()->GetPlayerData( *it );
		int ZoneId = pPlayer->DispData.ZoneId;
		if( pPlayer->DispData.CopyMapInstId != 1 )
			ZoneId = pPlayer->DispData.fZoneId;

		serverMap[ ZoneId ] = SERVER->GetServerManager()->GetGateServerByZoneId( nLineId, ZoneId );
	}

	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	stdext::hash_map<int,stServerInfo*>::iterator itt;

	for( itt = serverMap.begin(); itt != serverMap.end(); itt++ )
	{
		pHead->DestZoneId = itt->first;
		if( itt->second )
			SERVER->GetServerSocket()->Send( itt->second->SocketId, SendPacket );
	}
}

void CTeamManager::HandleBuildRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId = pHead->Id;
	int nPlayerIdTo = pHead->DestZoneId;

	int nErr = 0;

	DO_LOCK( Lock::Team );

	if( m_addTeamRecver[nPlayerIdTo].find( nPlayerId) != m_addTeamRecver[nPlayerIdTo].end() )
		return ;

	if( m_addTeamRecver[nPlayerIdTo].size() == MAX_TEAM_PENDING_COUNT )
	{
		SendError( nPlayerId, TEAM_NOTIFY_LIST_FULL );
		return ;
	}

	nErr = CheckBuildTeam( nPlayerId, nPlayerIdTo );

	if( nErr != NONE_ERROR )
	{
		SendError( nPlayerId, nErr );
		return ;
	}

	m_addTeamRecver[nPlayerIdTo].insert( nPlayerId );

	SERVER->GetPlayerManager()->SendSimpleInfoPacket( nPlayerIdTo, CLIENT_TEAM_BuildRequest, nPlayerId );	
}

// 转发拒绝
void CTeamManager::HandleRefused(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Team );
	
	int nType = pHead->DestZoneId;
	int nPlayer = pHead->Id;;
	int nPlayerTo = pHead->SrcZoneId;
	
	if( ( nType == TEAM_ACTION_BUILD || nType == TEAM_ACTION_ADD ) && m_addTeamRecver[nPlayer].find( nPlayerTo ) != m_addTeamRecver[nPlayer].end() )
	{
		m_addTeamRecver[nPlayer].erase( nPlayerTo );
		SendRefuse( nPlayer, nPlayerTo, nType );
	}
	else if( ( nType == TEAM_ACTION_JOIN ) && m_joinTeamRecver[nPlayer].find( nPlayerTo ) != m_joinTeamRecver[nPlayer].end() )
	{
		m_joinTeamRecver[nPlayer].erase( nPlayerTo );
		SendRefuse( nPlayer, nPlayerTo, nType );
	}
}

void CTeamManager::HandleAcceptBuildRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId =  pHead->Id;
	int nPlayerIdTo = pHead->DestZoneId;

	int nTeamId;
	DO_LOCK( Lock::Team | Lock::Player );

	// 没有发送过请求，这个响应请求是非法的
	if( m_addTeamRecver[nPlayerId].find(nPlayerIdTo) == m_addTeamRecver[nPlayerId].end() )
		return ;


	int nErr = BuildTeam( nPlayerIdTo, nPlayerId, nTeamId );
	if( nErr != NONE_ERROR )
	{
		SendError( nPlayerId, nErr );
		return;
	}

	// 通知两个玩家加入队伍(PS:要先发送玩家自己加入到队伍的消息)
	SendTeamInfo( nPlayerIdTo );
	SendTeamInfo( nPlayerId );

	stAccountInfo* pAInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( pAInfo )
		SendTeamInfoToZone( pAInfo->LineId, nTeamId );

	// 通知队伍所在的ZoneServer

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerIdTo );
	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( nPlayerIdTo );

	if( pPlayer.isNull() || !pAccountInfo )
		return ;

	ClearPendingList( nPlayerId );
	ClearPendingList( nPlayerIdTo );
}

void CTeamManager::SendRefuse( int nPlayerId, int nPlayerIdTo, int mType )
{
	DO_LOCK( Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerIdTo );
	if( !pAccountInfo )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_Refused, pAccountInfo->AccountId, SERVICE_CLIENT, mType, nPlayerId );
	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
}

void CTeamManager::SendCancel( int nPlayerId, int nPlayerIdTo, int mType )
{
	DO_LOCK( Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerIdTo );
	if( !pAccountInfo )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket, ClIENT_TEAM_Cancel, pAccountInfo->AccountId, SERVICE_CLIENT, mType, nPlayerId );
	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
}

void CTeamManager::SendBuildRequest( int nPlayerId, int nPlayerIdTo )
{
	SERVER->GetPlayerManager()->SendSimplePacket( nPlayerIdTo, CLIENT_TEAM_BuildRequest, nPlayerId, nPlayerIdTo );
}

void CTeamManager::HandleDisband(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Team );
	
	int nPlayerId = pHead->Id;

	DisbandTeamByPlayer( nPlayerId );
}

void CTeamManager::HandleDropRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Team | Lock::Player );

	int nPlayer = pHead->Id;
	int nPlayerTo = pHead->DestZoneId;

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayer );

	if( !pAccountInfo )
		return ;

	CTeam* pTeam = GetTeamByPlayer( nPlayer );

	// 检查该玩家是否在队伍中
	if( !pTeam || !pTeam->IsTeammate( nPlayerTo ) )
		return ;

	// 检查该玩家是否是队长
	if( !pTeam->IsCaption(nPlayer) )
	{
		SendError( nPlayer, TEAM_ERROR );
		return ;
	}

	// 通知队伍的所有玩家，该玩家已离开队伍
	std::list<int>::iterator it;
	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		NotifyPlayerLeave( *it, nPlayerTo );
	}

	NotifyZonePlayerLeave( pAccountInfo->LineId, nPlayerTo, pTeam->GetId() );

	LeaveTeam( nPlayerTo, pTeam->GetId() );
}

void CTeamManager::HandleLeaveRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId = pHead->Id;

	DO_LOCK( Lock::Team | Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );

	if( !pAccountInfo )
		return ;

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );

	// 检查玩家是否在队伍中
	if( !pTeam )
		return ;


	// 通知队伍的所有玩家,有玩家离开队伍
	std::list<int>::iterator it;
	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		NotifyPlayerLeave( *it, nPlayerId );
	}

	NotifyZonePlayerLeave( pAccountInfo->LineId, nPlayerId, pTeam->GetId() );

	LeaveTeam( nPlayerId, pTeam->GetId() );

	if( pTeam->m_teammate.size() == 0 )
		DisbandTeam( pTeam->GetId() );
}

void CTeamManager::HandleBuildAlongRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Team | Lock::Player );

	int nPlayerId = pHead->Id;

	PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( nPlayerId );
	stAccountInfo* pAccount = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );

	if( pPlayerData.isNull() || !pAccount )
		return ;

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );

	if( pTeam )
		return ;

	//pTeam = new CTeam();
	CMemPool::GetInstance()->AllocObj<CTeam>( pTeam );
	pTeam->m_nId = BuildTeamId();
	pTeam->Join( nPlayerId );
	sprintf( pTeam->m_szName, "%s的队伍", 	pPlayerData->BaseData.PlayerName);

	this->m_playerTeamMap[nPlayerId] = pTeam;
	this->m_teamMap[pTeam->GetId()] = pTeam;

#ifdef WORLDSERVER

	// 注册一个聊天队伍频道
	CChannel* pChannel = SERVER->GetChatManager()->GetChannelManager()->RegisterChannel( CHAT_MSG_TYPE_TEAM, pTeam->m_szName );
	int nChannelId = pChannel->GetId();
	pTeam->m_nChatChannelId = nChannelId;

	// 把创建队伍的玩家加入到聊天频道中
	SERVER->GetChatManager()->GetChannelManager()->AddPlayer( nPlayerId, nChannelId );

	// 暂时从聊天频道中移除玩家, 等待玩家主动加入到聊天频道中
	SERVER->GetChatManager()->GetChannelManager()->RemovePlayer( nPlayerId, nChannelId, false );

#endif

	// 发送队伍消息
	SendTeamInfo( nPlayerId );
	SendTeamInfoToZone( pAccount->LineId, pTeam->m_nId );
}

void CTeamManager::HandleJoinRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	DO_LOCK( Lock::Team );

	int nPlayerId = pHead->Id;
	int nTeam = pHead->DestZoneId;
	CTeam* pTeam = GetTeam( nTeam );

	if( !pTeam )
		return ;

	if( m_joinTeamRecver[pTeam->GetCaption()].find( nPlayerId) != m_joinTeamRecver[pTeam->GetCaption()].end() )
		return ;

	if(  m_joinTeamRecver[pTeam->GetCaption()].size() == MAX_TEAM_PENDING_COUNT )
	{
		SendError( nPlayerId,TEAM_NOTIFY_LIST_FULL );
		return ;
	}

	if( pTeam->m_teammate.size() == MAX_TEAMMATE_NUM )
	{
		SendError( nPlayerId, TEAM_TEAMMATE_FULL );
		return ;
	}

	m_joinTeamRecver[pTeam->GetCaption()].insert( nPlayerId );

	SERVER->GetPlayerManager()->SendSimpleInfoPacket( pTeam->GetCaption(), CLIENT_TEAM_JoinRequest, nPlayerId );
}

void CTeamManager::HandleAcceptJoinRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId = pHead->Id;
	int nPlayerIdTo = pHead->DestZoneId;

	DO_LOCK( Lock::Team | Lock::Player );

	// 没有发送过请求，这个响应请求是非法的
	if( m_joinTeamRecver[nPlayerId].find(nPlayerIdTo) == m_joinTeamRecver[nPlayerId].end() )
		return ;

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );

	if( !pAccountInfo )
		return ;

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( !pTeam )
		return ;

	if( !pTeam->IsCaption( nPlayerId ) )
		return ;

	int nErr;

	m_joinTeamRecver[nPlayerId].erase( nPlayerIdTo );

	nErr = JoinTeam( nPlayerIdTo, pTeam->GetId() );

	if( nErr != NONE_ERROR )
	{
		SendError( nPlayerId, nErr );
		return ;
	}

	SendTeamInfo( nPlayerIdTo );

	std::list<int>::iterator it;

	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		if( *it == nPlayerIdTo )
			continue;

		NotifyPlayerJoined( *it, nPlayerIdTo, pTeam->GetId() );
	}

	NotifyZonePlayerJoined( pAccountInfo->LineId, nPlayerIdTo, pTeam->GetId() );
}

void CTeamManager::HandleAcceptAddRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId = pHead->Id;
	int nPlayerIdTo = pHead->DestZoneId;

	DO_LOCK( Lock::Team | Lock::Player );

	// 没有发送过请求，这个响应请求是非法的
	if( m_addTeamRecver[nPlayerId].find(nPlayerIdTo) == m_addTeamRecver[nPlayerId].end() )
		return ;

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );

	if( !pAccountInfo )
		return ;

	CTeam* pTeam;

	pTeam = GetTeamByPlayer( nPlayerId );

	if( pTeam )
	{
		SendError( nPlayerId, TEAM_PLAYER_ERROR );
		return ;
	}

	pTeam = GetTeamByPlayer( nPlayerIdTo );

	if( !pTeam )
	{
		SendError( nPlayerId, TEAM_ERROR );
		return ;
	}

	if( !pTeam->IsCaption( nPlayerIdTo ) )
	{
		SendError( nPlayerId, TEAM_PLAYER_ERROR );
		return ;
	}

	m_addTeamRecver[nPlayerId].erase( nPlayerIdTo );

	int nErr = JoinTeam( nPlayerId, pTeam->GetId() );

	if( nErr != NONE_ERROR )
	{
		SendError( nPlayerId, nErr );
		return ;
	}

	SendTeamInfo( nPlayerId );

	std::list<int>::iterator it;

	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		if( *it == nPlayerId )
			continue;

		NotifyPlayerJoined( *it, nPlayerId, pTeam->GetId() );
	}

	NotifyZonePlayerJoined( pAccountInfo->LineId, nPlayerId, pTeam->GetId() );
}

void CTeamManager::HandleAddRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId = pHead->Id;
	int nPlayerIdTo = pHead->DestZoneId;

	DO_LOCK( Lock::Team );

	CTeam* pTeam;

	pTeam = GetTeamByPlayer( nPlayerIdTo );

	if( pTeam )
	{
		SendError( nPlayerId, TEAM_PLAYER_ERROR );
		return ;
	}

	pTeam= GetTeamByPlayer( nPlayerId );
	if( !pTeam )
		return ;

	if( m_addTeamRecver[nPlayerIdTo].find( nPlayerId) != m_addTeamRecver[nPlayerIdTo].end() )
		return ;

	if( m_addTeamRecver[nPlayerIdTo].size() == MAX_TEAM_PENDING_COUNT )
	{
		SendError( nPlayerId,TEAM_NOTIFY_LIST_FULL );
		return ;
	}

	if( pTeam->m_teammate.size() == MAX_TEAMMATE_NUM )
	{
		SendError( nPlayerId, TEAM_TEAMMATE_FULL );
		return ;
	}

	m_addTeamRecver[nPlayerIdTo].insert( nPlayerId );

	if( pTeam->IsCaption( nPlayerId ) )
		SERVER->GetPlayerManager()->SendSimpleInfoPacket( nPlayerIdTo, CLIENT_TEAM_AddRequest, nPlayerId, pTeam->GetTeammate()->size() );
}

void CTeamManager::HandleMapMark(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{

	int nPlayer = pHead->Id;

	DO_LOCK( Lock::Team | Lock::Player );

	CTeam* pTeam = GetTeamByPlayer( nPlayer );

	if( !pTeam || pTeam->GetCaption() != nPlayer )
		return ;

	pTeam->mapMark[pHead->DestZoneId].ReadPacket( Packet );


	CMemGuard buf( MAX_PACKET_SIZE ); 
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_MapMark, 0, SERVICE_CLIENT, pHead->DestZoneId  );
	pTeam->mapMark[pHead->DestZoneId].WritePacket( &SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	stAccountInfo* pAccountInfo;

	std::list<int>::iterator it;
	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( *it );
		if( pAccountInfo )
		{
			pSendHead->Id = pAccountInfo->AccountId;
			SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
		}
	}
}

void CTeamManager::HandleTargetMark(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{

	int nPlayer = pHead->Id;

	DO_LOCK( Lock::Team | Lock::Player );

	CTeam* pTeam = GetTeamByPlayer( nPlayer );

	if( !pTeam || pTeam->GetCaption() != nPlayer )
		return ;

	pTeam->targetMark[pHead->DestZoneId].ReadPacket( Packet );


	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_TargetMark, 0, SERVICE_CLIENT, pHead->DestZoneId );
	pTeam->targetMark[pHead->DestZoneId].WritePacket( &SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	stAccountInfo* pAccountInfo;

	std::list<int>::iterator it;
	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{


		pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( *it );
		if( pAccountInfo )
		{
			pSendHead->Id = pAccountInfo->AccountId;
			SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
		}
	}
}

void CTeamManager::HandleFollowRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int playerId = pHead->Id;

	DO_LOCK( Lock::Team | Lock::Player );

	CTeam* pTeam = GetTeamByPlayer( playerId );

	if( !pTeam )
		return ;

	if( !pTeam->IsCaption( playerId ) )
		return;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, CLIENT_TEAM_Follow, 0, SERVICE_CLIENT, pHead->DestZoneId );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	stAccountInfo* pAccountInfo;

	std::list<int>::iterator it;
	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		//if( *it == playerId )
		//	continue;

		pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( *it );

		if( pAccountInfo && pAccountInfo->socketId == SocketHandle )
		{
			pSendHead->Id = pAccountInfo->AccountId;
			SERVER->GetServerSocket()->Send( pAccountInfo->socketId, SendPacket );
		}
	}
}

void CTeamManager::HandleBaseInfo(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
{
	int nPlayerId = pHead->Id;
	
	DO_LOCK( Lock::Team | Lock::Player | Lock::Server );

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( !pTeam )
		return ;
	
	if( pTeam->GetCaption() != nPlayerId )
		return ;

	char teamName[255] = { 0 };
	int nNewTeamLeader = 0;
	char cU = pHead->DestZoneId;
	char cV = pHead->SrcZoneId;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_TEAM_BaseInfo, 0, SERVICE_CLIENT, 0, cU, cV );

	if( Packet->readFlag() )
	{
		Packet->readString( teamName ,255);
		strcpy_s( pTeam->m_szName,MAX_TEAM_NAME_LENGTH, teamName );
		sendPacket.writeFlag( true );
		sendPacket.writeString( teamName );
	}
	else
	{
		sendPacket.writeFlag( false );
	}

	if( Packet->readFlag() )
	{
		nNewTeamLeader = Packet->readInt( 32 );
		pTeam->SetCaption( nNewTeamLeader );
		sendPacket.writeFlag( true );
		sendPacket.writeInt( nNewTeamLeader, 32 );
	}
	else
	{
		sendPacket.writeFlag( false );
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	std::list<int>::iterator it;
	stAccountInfo* pAccountInfo;
	for( it=pTeam->m_teammate.begin(); it!=pTeam->m_teammate.end(); it++ )
	{
		pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( *it );
		if( !pAccountInfo )
			continue;

		pSendHead->Id = pAccountInfo->AccountId;

		SERVER->GetServerSocket()->Send( pAccountInfo->socketId, sendPacket );
	}

	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( pTeam->GetCaption() );

	if( pPlayer.isNull() )
		return ;

	stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServerByZoneId( pAccountInfo->LineId, pPlayer->DispData.ZoneId );

	pSendHead->DestServerType = SERVICE_ZONESERVER;
	pSendHead->DestZoneId = pPlayer->DispData.ZoneId;
	pSendHead->Id = pTeam->GetId();

	if( pServerInfo )
		SERVER->GetServerSocket()->Send( pServerInfo->SocketId, sendPacket );
}

void CTeamManager::ClearPendingList( int nPlayerId )
{
	DO_LOCK( Lock::Team );

	// 向所有邀请该玩家的玩家发送拒绝消息, 向该玩家发出邀请的所有玩家发送取消邀请消息 [2009-6-3 LivenHotch]
	std::list<int>::iterator it;

	// 发送拒绝
	for( it = m_joinTeamRecver[nPlayerId].begin(); it != m_joinTeamRecver[nPlayerId].end(); it++ )
	{
		SendRefuse( nPlayerId, *it, TEAM_ACTION_JOIN );
	}

	m_joinTeamRecver[nPlayerId].clear();

	for( it = m_addTeamRecver[nPlayerId].begin(); it != m_addTeamRecver[nPlayerId].end(); it++ )
	{
		SendRefuse( nPlayerId, *it, TEAM_ACTION_ADD );
	}

	m_addTeamRecver[nPlayerId].clear();
}

void CTeamManager::HandleInfoRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;
	int nTeamId = pHead->DestZoneId;

	DO_LOCK( Lock::Team | Lock::Player );

	CTeam* pTeam = GetTeam( nTeamId );
	if( !pTeam )
		return ;

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( nPlayerId );
	if( !pAccountInfo )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_TEAM_ShowTeamInfo, pAccountInfo->AccountId, SERVICE_CLIENT );

	sendPacket.writeString( pTeam->m_szName );
	sendPacket.writeInt( pTeam->m_teammate.size(), 32 );
	PlayerDataRef pPlayerData;
	
	std::list<int>::iterator it;

	for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
	{
		pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( *it );
		if( !pPlayerData.isNull() )
		{
			sendPacket.writeString( pPlayerData->BaseData.PlayerName );
			sendPacket.writeInt( pPlayerData->DispData.Family, 8 );
			sendPacket.writeInt( pPlayerData->DispData.Level, 8 );
		}
		else
		{
			sendPacket.writeString( "" );
			sendPacket.writeInt( 0, 8 );
			sendPacket.writeInt( 0, 8 );
		}
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}

void CTeamManager::addPlayerToTeamChannel( int nPlayerId )
{
	CTeam* pTeam = GetTeamByPlayer( nPlayerId );
	if( pTeam == NULL )
		return ;

	// 将新玩家加入到聊天频道中
	SERVER->GetChatManager()->GetChannelManager()->AddPlayer( nPlayerId, pTeam->GetChatChannelId() );
	 //SERVER->GetChatManager()->GetChannelManager()->RemovePlayer( nPlayerId, pTeam->GetChatChannelId(), false );
}

void CTeamManager::HandleInvationCopymap( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;

	int nCopymap = pHead->DestZoneId;

	DO_LOCK( Lock::Team | Lock::Player );

	CTeam* pTeam = GetTeamByPlayer( nPlayerId );

	if( pTeam == NULL )
		return ;

	std::list<int>::iterator it;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_TEAM_InvationCopymap, 0, SERVICE_CLIENT, nCopymap );
	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	stAccountInfo* pInfo;

	// 向自己之外的所有人发送进副本邀请
	for( it = pTeam->GetTeammate()->begin(); it != pTeam->GetTeammate()->end(); it++ )
	{
		if( *it == nPlayerId )
			continue;
		
		pInfo = SERVER->GetPlayerManager()->GetAccountInfo( *it );

		if( pInfo == NULL )
			continue;

		pSendHead->Id = pInfo->AccountId; 

		SERVER->GetServerSocket()->Send( pInfo->socketId, sendPacket );
	}
}