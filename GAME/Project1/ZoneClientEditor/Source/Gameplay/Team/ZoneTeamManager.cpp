#include "../ServerGameplayState.h"
#include "../GameObjects/PlayerObject.h"
#include "ZoneTeamManager.h"
#include "Gameplay/Mission/PlayerMission.h"

CZoneTeamManager g_ZoneTeamManager;

CZoneTeam::CZoneTeam( int nId /* = 0 */ )
{
	m_nId = nId;
}

int CZoneTeam::GetTeamId()
{
	return m_nId;
} 

int CZoneTeam::GetCaption()
{
	if( m_playerList.size() == 0 )
		return 0;

	return *m_playerList.begin();
}

// 设置队长,此队员必须在队伍内
void CZoneTeam::SetCaption( int nPlayerId )
{
	if( m_playerList.size() == 0 )
		return ;

	if( !IsTeammate( nPlayerId ) )
		return ;

	int nCaption = *m_playerList.begin();

	std::list<int>::iterator it;
	it = FindPlayer( nPlayerId );

	*it = nCaption;

	*m_playerList.begin() = nPlayerId;
}

void CZoneTeam::SetTeamName( std::string strName )
{
	m_strName = strName;
}

void CZoneTeam::GetTeamName( char* strName, U32 szSize )
{
	if( !strName )
		return ;
	dStrcpy( strName, szSize, m_strName.c_str() );
}

bool CZoneTeam::IsTeammate( int nPlayerId )
{
	return FindPlayer( nPlayerId ) != m_playerList.end();
}

// 此方法内部调用,不加锁
std::list<int>::iterator CZoneTeam::FindPlayer( int nPlayerId )
{
	std::list<int>::iterator it;

	for( it = m_playerList.begin(); it != m_playerList.end(); it++ )
		if( *it == nPlayerId )
			break;

	return it;
}

int CZoneTeam::GetTeammateCount()
{
	return m_playerList.size();
}

void CZoneTeam::AddPlayer( int nPlayerId )
{
	// 玩家已经在队伍中
	if( FindPlayer( nPlayerId ) != m_playerList.end() )
		return ;	
	
	m_playerList.push_back( nPlayerId );

	//for each( int playerId in m_playerList )
	//{
	//	Player* pPlayer = g_ServerGameplayState->GetPlayer( playerId );
	//	if( pPlayer )
	//	{
	//		NetConnection* pConn = 
	//	}
	//}
	

}

void CZoneTeam::RemovePlayer( int nPlayerId )
{
	m_playerList.erase( FindPlayer( nPlayerId ) );
}

// 通过队员序号找到相应的队员
int CZoneTeam::GetPlayerId( int nIndex )
{
	std::list<int>::iterator it;
	int i = 0;
	for( it = m_playerList.begin(); it != m_playerList.end(); it++, i++ )
		if( i == nIndex )
			return *it;
	return 0;
}

// ----------------------------------------------------------------------------
// 添加任务到队伍共享任务列表中
void CZoneTeam::AddShareMission(S32 iMissionID)
{
	if(iMissionID < 0 || iMissionID >= MISSION_MAX)
		return;
	stdext::hash_map<S32, S32>::iterator itFind = m_ShareMissionList.find(iMissionID);
	if(itFind != m_ShareMissionList.end())
		itFind->second ++;
	else
		m_ShareMissionList.insert(std::make_pair(iMissionID, 1));
}

// ----------------------------------------------------------------------------
// 从队伍共享任务列表中删除任务
void CZoneTeam::DelShareMission(S32 iMissionID)
{
	if(iMissionID < 0 || iMissionID >= MISSION_MAX)
		return;
	stdext::hash_map<S32, S32>::iterator itFind = m_ShareMissionList.find(iMissionID);
	if(itFind != m_ShareMissionList.end())
	{
		itFind->second --;
		if(itFind->second <= 0)
			m_ShareMissionList.erase(itFind);
	}
}

// ----------------------------------------------------------------------------
// 为玩家获取队伍共享任务列表
bool CZoneTeam::GetShareMission(int nPlayerId)
{
	Player* player = g_ServerGameplayState->GetPlayer(nPlayerId);
	if(!player)
		return false;
	
	player->missionShareList.clear();

	stdext::hash_map<S32, S32>::iterator itB = m_ShareMissionList.begin();
	S32 idPos;
	for(; itB != m_ShareMissionList.end(); ++itB)
	{
		S32 iMissionID = itB->second;
		if(g_MissionManager->IsFinishedMission(player, iMissionID) ||
			g_MissionManager->IsAcceptedMission(player, iMissionID,idPos))
		{
			continue;
		}
		player->missionShareList.push_back(iMissionID);
	}
	return true;
}

CZoneTeamManager::CZoneTeamManager()
{

}

CZoneTeamManager::~CZoneTeamManager()
{
	stdext::hash_map<int,CZoneTeam*>::iterator it;

	for( it = m_idTeamMap.begin(); it != m_idTeamMap.end(); it++ )
	{
		delete it->second;
	}
}

CZoneTeam* CZoneTeamManager::GetTeam( int nTeamId )
{
	stdext::hash_map<int,CZoneTeam*>::iterator it;

	it = m_idTeamMap.find( nTeamId );
	if( it != m_idTeamMap.end() )
		return it->second;

	return NULL;
}

CZoneTeam* CZoneTeamManager::GetTeamByPlayerId( int nPlayerId )
{
	stdext::hash_map<int,CZoneTeam*>::iterator it;

	it = m_playerTeamMap.find( nPlayerId );
	if( it != m_playerTeamMap.end() )
		return it->second;

	return NULL;
}

void CZoneTeamManager::OnTeamBaseInfoNotify( stPacketHead* pHead, Base::BitStream& packet )
{
	int nTeamId = pHead->Id;
	char cU = pHead->SrcZoneId;
	char cV = pHead->LineServerId;
	Player* pPlayer;

	CZoneTeam* pTeam = GetTeam( nTeamId );

	if( !pTeam )
		return ;

	if( cU != 0 )
		pTeam->m_cU = cU;
	if( cV != 0 )
		pTeam->m_cV = cV;

	char szName[255] ={0,};
	if( packet.readFlag() )
	{
		packet.readString( szName,255 );
		pTeam->m_strName = szName;
	}

	int nNewCaption;
	if( packet.readFlag() )
	{
		nNewCaption = packet.readInt( 32 );
		pPlayer = g_ServerGameplayState->GetPlayer( pTeam->GetCaption() );
		if( pPlayer )
		{
			pPlayer->getTeamInfo().m_bIsCaption = false;
			pPlayer->setUpdateTeam();
		}
		pTeam->SetCaption( nNewCaption );
		pPlayer = g_ServerGameplayState->GetPlayer( nNewCaption );
		if( pPlayer )
		{
			pPlayer->getTeamInfo().m_bIsCaption = true;
			pPlayer->setUpdateTeam();
		}
	}
	std::list<int>::iterator it;

	for( it= pTeam->m_playerList.begin(); it != pTeam->m_playerList.end(); it++ )
	{
		pPlayer = g_ServerGameplayState->GetPlayer( *it );
		if( !pPlayer )
			continue ;

		dStrcpy( pPlayer->getTeamInfo().m_szName, sizeof(pPlayer->getTeamInfo().m_szName), szName );
		pPlayer->setUpdateTeam();
	}

}


// 队伍通知
void CZoneTeamManager::OnTeamInfoNotify( stPacketHead* pHead, Base::BitStream& packet )
{
	int nTeamId = pHead->Id;
	bool isSquad = packet.readFlag();
	char szName[COMMON_STRING_LENGTH];
	packet.readString( szName ,COMMON_STRING_LENGTH);

	int i;
	CZoneTeam* pTeam;
	Player* pPlayer;
	// 检查是否有存在的队伍

	pTeam = GetTeam( nTeamId );

	if( pTeam )
	{	
		for( i = 0; i < pTeam->GetTeammateCount(); i++ )
		{
			//OnTeamPlayerLeave( pTeam->GetPlayerId( i ), pTeam->GetTeamId() );

			m_playerTeamMap.erase( pTeam->GetPlayerId(i) );

			pPlayer = g_ServerGameplayState->GetPlayer( pTeam->GetPlayerId( i ) );

			if( !pPlayer )
				continue;

			stTeamInfo& mTeamInfo = pPlayer->getTeamInfo();
			mTeamInfo.m_nId = 0;
			*(mTeamInfo.m_szName) = 0;
			mTeamInfo.m_bIsCaption =  false;
			mTeamInfo.m_bInTeam = false;

			pPlayer->setUpdateTeam();
		}

		OnTeamDisband( pTeam->GetTeamId() );
	}

	pTeam = new CZoneTeam( nTeamId );
	m_idTeamMap[nTeamId] = pTeam;
	pTeam->SetTeamName( szName );
	int nCount = packet.readInt( 32 );

	int nPlayerId;


	for( i = 0; i < nCount; i++ )
	{
		nPlayerId = packet.readInt( 32 );
		OnTeamPlayerJoin( nPlayerId, nTeamId );
	}
}

// 玩家进入队伍
void CZoneTeamManager::OnTeamPlayerJoin( int nPlayerId, int nTeamId )
{
	// 玩家没有加入队伍
	if( nTeamId == 0 )
		return ;

	CZoneTeam* pTeam;
	pTeam = GetTeamByPlayerId( nPlayerId );

	// 玩家已在队伍中
	if( pTeam )
		return ;

	pTeam = GetTeam( nTeamId );

	// 队伍不存在
	if( !pTeam )
	{
		AssertWarn( false, "指定的队伍不存在" );
		return ;
	}

	Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );
	if( pPlayer )
	{
		stTeamInfo& mTeamInfo = pPlayer->getTeamInfo();
		if( pTeam->GetTeammateCount() == 0 )
			mTeamInfo.m_bIsCaption = true;
		else
			mTeamInfo.m_bIsCaption = false;
		pTeam->GetTeamName( mTeamInfo.m_szName, sizeof(mTeamInfo.m_szName) );
		mTeamInfo.m_nId = nTeamId;
		mTeamInfo.m_bInTeam = true;
		pPlayer->setUpdateTeam();

		// 添加当前新入队员的任务到共享任务列表中
		for(S32 j = 0; j < MISSION_ACCEPT_MAX; ++j)
			pTeam->AddShareMission(pPlayer->missionInfo.Flag[j].MissionID);
	}

	pTeam->AddPlayer( nPlayerId );
	m_playerTeamMap[ nPlayerId ] = pTeam;
}

// 玩家离开队伍
void CZoneTeamManager::OnTeamPlayerLeave( int nPlayerId, int nTeamId )
{
	// 玩家没在队伍中
	if( nTeamId == 0 )
		return ;

	CZoneTeam* pTeam = GetTeamByPlayerId( nPlayerId );
	if( !pTeam )
		return ;

	Player* pPlayer = g_ServerGameplayState->GetPlayer( nPlayerId );
	if( pPlayer )
	{
		stTeamInfo& mTeamInfo = pPlayer->getTeamInfo();
		mTeamInfo.m_nId = 0;
		*(mTeamInfo.m_szName) = 0;
		mTeamInfo.m_bIsCaption =  false;
		mTeamInfo.m_bInTeam = false;
		pPlayer->setUpdateTeam();

		// 从共享任务列表中删除当前玩家所共享的任务
		for(S32 j = 0; j < MISSION_ACCEPT_MAX; ++j)
			pTeam->DelShareMission(pPlayer->missionInfo.Flag[j].MissionID);
	}

	pTeam->RemovePlayer( nPlayerId );
	pPlayer = g_ServerGameplayState->GetPlayer( pTeam->GetCaption() );
	if( pPlayer )
	{
		pPlayer->getTeamInfo().m_bIsCaption = true;
		pPlayer->setUpdateTeam();
	}

	m_playerTeamMap.erase( nPlayerId );
}

// 玩家进入zone
void CZoneTeamManager::OnPlayerEnterZone( int nPlayerId, int nTeamId )
{
	if( nTeamId == 0 )
		return ;
}

// 玩家离开zone
void CZoneTeamManager::OnPlayerLeaveZone( int nPlayerId )
{
	CZoneTeam* pTeam = GetTeamByPlayerId( nPlayerId );

	// 玩家不在队伍中
	if( !pTeam )
		return ;

	OnTeamPlayerLeave( nPlayerId, pTeam->GetTeamId() );

	// 队伍人数空了,干掉这个队伍
	if( pTeam->GetTeammateCount() == 0 )
	{
		OnTeamDisband( pTeam->GetTeamId() );
	}
}

void CZoneTeamManager::OnTeamDisband( int nTeamId )
{
	CZoneTeam* pTeam = GetTeam( nTeamId );

	if( !pTeam )
		return ;

	m_idTeamMap.erase( nTeamId );

	delete pTeam;
}

ConsoleFunction( SvrIsMyselfHasTeam, bool, 2, 2, "SvrIsMyselfHasTeam( %PlayerID )" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return false;

	return pPlayer->mTeamInfo.m_bInTeam;
}

ConsoleFunction( SvrIsMyselfCaption, bool, 2, 2, "SvrIsMyselfCaption( %PlayerID )" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return false;

	return pPlayer->mTeamInfo.m_bIsCaption;
}

ConsoleFunction( SvrIsAllTeammateNearby, bool, 3, 3, "SvrIsAllTeammateNearby( %PlayerID, %Distance )" )
{

	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return false;

	int distance = atoi( argv[2] );

	if( !pPlayer->mTeamInfo.m_bInTeam )
		return false;

	CZoneTeam* pTeam = ZONE_TEAM_MGR->GetTeamByPlayerId( pPlayer->getPlayerID() );
	if( pTeam )
		return false;

	for( int i = 0; i < pTeam->GetTeammateCount(); i++ )
	{
		int pid = pTeam->GetPlayerId( i );

		if( pid == pPlayer->getPlayerID() )
			continue;

		Player* pDestPlayer = g_ServerGameplayState->GetPlayer( pid );

		if( !pDestPlayer )
			return false;

		if( pDestPlayer->getDistance( pPlayer ) > distance )
			return false;
	}

	return true;
}

ConsoleFunction( SvrGetTeammate, const char*, 2, 2, "SvrGetTeammate( %PlayerID )" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] )) ;
	if( !pPlayer )
		return Con::getReturnBuffer("0");

	if( !pPlayer->mTeamInfo.m_bInTeam )
		return Con::getReturnBuffer("0");

	CZoneTeam* pTeam = ZONE_TEAM_MGR->GetTeamByPlayerId( pPlayer->getPlayerID() );
	if( pTeam )
		return Con::getReturnBuffer("0");

	std::string teammate;

	for( int i = 0; i < pTeam->GetTeammateCount(); i++ )
	{
		int pid = pTeam->GetPlayerId( i );

		Player* pDestPlayer = g_ServerGameplayState->GetPlayer( pid );

		if( !pDestPlayer )
			teammate += "0";
		else
			teammate += Con::getIntArg( pDestPlayer->getId() );

		teammate += " ";
	}

	return Con::getReturnBuffer( teammate.c_str() );
}